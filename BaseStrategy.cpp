#include "StrategyManager.h"
#include "BaseStrategy.h"
#include "glog/logging.h"

void BaseStrategy::sendOrder(OrderItem& _order_item){
    next_req.clear();
    next_req.m_type = req_type::NEWORDER;
    next_req.m_req_price = _order_item.m_order_price;
    next_req.m_order_quant = _order_item.m_order_quant;
    next_req.m_total_fills = 0;
    next_req.m_order_side = _order_item.m_side;
    next_req.m_order_id = mShmemManager->getNextOrderID();
    next_req.m_symbolId = sym_man->getSymbolID();
    next_req.m_order_type = _order_item.m_type;
    mShmemManager->pushReq(next_req);

    _order_item.m_id = next_req.m_order_id;
    _order_item.m_timestamp = sym_man->getCurrentTime();
    _order_item.m_state = order_state::PENDING_NEW;
    strat_man->trackOrder(next_req.m_order_id, m_strat_id);
}

void BaseStrategy::modOrder(OrderItem& _order_item){
    next_req.clear();
    next_req.m_type = req_type::MODORDER;
    next_req.m_req_price = _order_item.m_order_price;
    next_req.m_order_quant = _order_item.m_order_quant;
    next_req.m_total_fills = 0;
    next_req.m_order_side = _order_item.m_side;
    next_req.m_order_id = _order_item.m_id;
    next_req.m_symbolId = sym_man->getSymbolID();
    next_req.m_order_type = _order_item.m_type;
    mShmemManager->pushReq(next_req);

    _order_item.m_id = next_req.m_order_id;
    _order_item.m_timestamp = sym_man->getCurrentTime();
    _order_item.m_state = order_state::PENDING_MODIFY;
    strat_man->trackOrder(next_req.m_order_id, m_strat_id);
}

void BaseStrategy::sendCancel(OrderItem& _order_item){
    next_req.clear();
    next_req.m_type = req_type::CANCEL;
    next_req.m_req_price = _order_item.m_order_price;
    next_req.m_order_quant = _order_item.m_order_quant;
    next_req.m_total_fills = _order_item.m_total_fills;
    next_req.m_order_side = _order_item.m_side;
    next_req.m_order_id = _order_item.m_id;
    next_req.m_symbolId = sym_man->getSymbolID();
    mShmemManager->pushReq(next_req);

    _order_item.m_timestamp = sym_man->getCurrentTime();
    _order_item.m_state = order_state::PENDING_CANCEL;
}

void BaseStrategy::processResp(const Response& _new_response){
    if(_new_response.m_type == resp_type::TRADE_CONFIRM){
        processRMFill(_new_response.m_side, _new_response.m_resp_price, _new_response.m_resp_quant);
    }
}

void BaseStrategy::processRMFill(const side& _side, const Price& _price, const Shares& _shares){
    mRMManager->subtractExposure(m_exposure);
    if(_side == side::BUY)
    {
        m_strat_position += _shares;
        sym_man->m_sym_position += _shares;
        m_exposure -= _price * _shares;

        next_log.clear();
        next_log.m_type = log_type::TRADE;
        next_log.m_side = side::BUY;
        next_log.m_price = _price;
        next_log.m_shares = _shares;
        next_log.m_symbolId = sym_man->getSymbolID();
        next_log.m_stratID = m_strat_id;
        next_log.m_current_time = sym_man->getCurrentTime();

        mShmemManager->pushLog(next_log);
    }
    else if(_side == side::SELL)
    {
        m_strat_position -= _shares;
        sym_man->m_sym_position -= _shares;
        m_exposure += _price * _shares;

        next_log.clear();
        next_log.m_type = log_type::TRADE;
        next_log.m_side = side::SELL;
        next_log.m_price = _price;
        next_log.m_shares = _shares;
        next_log.m_symbolId = sym_man->getSymbolID();
        next_log.m_stratID = m_strat_id;
        next_log.m_current_time = sym_man->getCurrentTime();

        mShmemManager->pushLog(next_log);
    }

    if(m_strat_position == 0){
        mRMManager->subtractPNL(m_pnl);
        m_pnl += m_exposure;
        m_exposure = 0;
        mRMManager->addPNL(m_pnl);
    }
    mRMManager->addExposure(m_exposure);
}

void BaseStrategy::setTimeout(const Timestamp& _time){
    mTimeManager->addTimeout(_time, sym_man->getSymbolID(), m_strat_id);
}

void BaseStrategy::passiveCover(){
    if(pcover_order.m_state == order_state::NONE){ 
        sendPCover();
        return;
    }

    if(m_strat_position > 0){
        if(sym_man->getAskPrice() < pcover_order.m_order_price) cancelPCover();
    }
    else if(m_strat_position < 0){
        if(sym_man->getBidPrice() > pcover_order.m_order_price) cancelPCover();
    }
}

bool BaseStrategy::pcoverOrderCanceled(){
    if(pcover_order.m_state == order_state::NONE) return true;
    return false;
}

void BaseStrategy::cancelPCover(){
    if(pcover_order.m_state == order_state::CONFIRMED) sendCancel(pcover_order);
}

void BaseStrategy::sendPCover(){
    pcover_order.m_order_quant = abs(m_strat_position);
    if(pcover_order.m_order_quant > 500) pcover_order.m_order_quant = 500;

    if(m_strat_position > 0){
        pcover_order.m_side = side::SELL;
        pcover_order.m_order_price = sym_man->getAskPrice();
    }
    else if(m_strat_position < 0){
        pcover_order.m_side = side::BUY;
        pcover_order.m_order_price = sym_man->getBidPrice();
    }
    
    if(pcover_order.m_order_quant == 0) return;

    pcover_order.m_type = order_type::HIDDENNASD;

    sendOrder(pcover_order);
}

MyOrderId BaseStrategy::getPCoverOrderID(){
    return pcover_order.m_id;
}
