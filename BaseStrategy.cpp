#include "StrategyManager.h"
#include "BaseStrategy.h"
#include "glog/logging.h"

void BaseStrategy::sendOrder(OrderItem& _order_item){

    next_req.m_req_price = _order_item.m_order_price;
    next_req.m_order_id = mShmemManager->getNextOrderID(m_thread_id);
    next_req.m_order_quant = _order_item.m_order_quant;
    next_req.m_total_fills = 0;
    next_req.m_symbolId = m_sym_id;
    next_req.m_order_side = _order_item.m_side;
    next_req.m_type = req_type::NEWORDER;
    next_req.m_order_type = _order_item.m_type;

    mShmemManager->pushReq(next_req, m_thread_id);

    _order_item.m_id = next_req.m_order_id;
    _order_item.m_timestamp = sym_man->getCurrentTime();
    _order_item.m_state = order_state::PENDING_NEW;
    strat_man->trackOrder(next_req.m_order_id, m_strat_id, m_thread_id);
}

void BaseStrategy::modOrder(OrderItem& _order_item){

    next_req.m_req_price = _order_item.m_order_price;
    next_req.m_order_id = _order_item.m_id;
    next_req.m_order_quant = _order_item.m_order_quant;
    next_req.m_total_fills = 0;
    next_req.m_symbolId = m_sym_id;
    next_req.m_order_side = _order_item.m_side;
    next_req.m_type = req_type::MODORDER;
    next_req.m_order_type = _order_item.m_type;

    mShmemManager->pushReq(next_req, m_thread_id);

    _order_item.m_timestamp = sym_man->getCurrentTime();
    _order_item.m_state = order_state::PENDING_MODIFY;
}

void BaseStrategy::sendCancel(OrderItem& _order_item){

    next_req.m_req_price = _order_item.m_order_price;
    next_req.m_order_id = _order_item.m_id;
    next_req.m_order_quant = _order_item.m_order_quant;
    next_req.m_total_fills = _order_item.m_total_fills;
    next_req.m_symbolId = m_sym_id;
    next_req.m_order_side = _order_item.m_side;
    next_req.m_type = req_type::CANCEL;
    next_req.m_order_type = order_type::NONE;

    mShmemManager->pushReq(next_req, m_thread_id);

    _order_item.m_timestamp = sym_man->getCurrentTime();
    _order_item.m_state = order_state::PENDING_CANCEL;
}

void BaseStrategy::processResp(const Response& _new_response){
    if(_new_response.m_type == resp_type::TRADE_CONFIRM){
        processRMFill(_new_response.m_side, _new_response.m_resp_price, _new_response.m_resp_quant);
    }
}

void BaseStrategy::processRMFill(const side& _side, const Price& _price, const Shares& _shares){

    // const Price old_exposure = m_exposure;

    next_log.m_price = _price;
    next_log.m_current_time = sym_man->getCurrentTime();
    next_log.m_shares = _shares;
    next_log.m_symbolId = m_sym_id;
    next_log.m_stratID = m_strat_id;
    next_log.m_side = _side;
    next_log.m_type = log_type::TRADE;

    int mult = (_side == side::BUY)?1:-1;

    const Shares share_mult = _shares * mult;

    m_strat_position += share_mult;
    sym_man->m_sym_position += share_mult;
    m_exposure -= _price * share_mult;

    mShmemManager->pushLog(next_log, m_thread_id);

    if(m_strat_position == 0){
        // mRMManager->subtractPNL(m_pnl);
        m_pnl += m_exposure;
        m_exposure = 0;
        // mRMManager->addPNL(m_pnl);
    }

    // mRMManager->subtractExposure(old_exposure - m_exposure);
}

void BaseStrategy::setTimeout(const Timestamp& _time){
    mTimeManager->addTimeout(_time, m_sym_id, m_strat_id, m_thread_id);
}

void BaseStrategy::passiveCover(bool _use_midpoint, Shares _desired_pos){
    if(pcover_order.m_state == order_state::NONE){ 
        sendPCover(_use_midpoint, _desired_pos);
        return;
    }

    if(_use_midpoint){
        if(m_strat_position > _desired_pos){
            if(sym_man->getMidPoint() < pcover_order.m_order_price) cancelPCover();
        }
        else if(m_strat_position < _desired_pos){
            if(sym_man->getMidPoint() > pcover_order.m_order_price) cancelPCover();
        }
    }
    else{
        if(m_strat_position > _desired_pos){
            if(sym_man->getAskPrice() < pcover_order.m_order_price) cancelPCover();
        }
        else if(m_strat_position < _desired_pos){
            if(sym_man->getBidPrice() > pcover_order.m_order_price) cancelPCover();
        }
    }
}

bool BaseStrategy::pcoverOrderCanceled(){
    if(pcover_order.m_state == order_state::NONE) return true;
    return false;
}

void BaseStrategy::cancelPCover(){
    if(pcover_order.m_state == order_state::CONFIRMED) sendCancel(pcover_order);
}

void BaseStrategy::sendPCover(bool _use_midpoint, Shares _desired_pos){
    const Shares order_size = _desired_pos - m_strat_position;

    pcover_order.m_order_quant = abs(order_size);
    if(pcover_order.m_order_quant > 500) pcover_order.m_order_quant = 500;

    if(order_size > 0){
        pcover_order.m_side = side::BUY;
        pcover_order.m_order_price = sym_man->getBidPrice() + CENTS;
    }
    else if(order_size < 0){
        pcover_order.m_side = side::SELL;
        pcover_order.m_order_price = sym_man->getAskPrice() - CENTS;
    }

    if(_use_midpoint){
        pcover_order.m_order_price = sym_man->getMidPoint();
        if(pcover_order.m_order_price % CENTS == 50){
            if(order_size < 0){
                pcover_order.m_order_price -= 50;
            }
            else if(order_size > 0){
                pcover_order.m_order_price += 50;
            }
        }
    }
    
    if(pcover_order.m_order_quant == 0) return;

    pcover_order.m_type = order_type::HIDDENSMART;

    sendOrder(pcover_order);
}

MyOrderId BaseStrategy::getPCoverOrderID(){
    return pcover_order.m_id;
}
