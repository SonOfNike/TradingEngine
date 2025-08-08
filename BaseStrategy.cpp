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
    mShmemManager->pushReq(next_req);

    _order_item.m_id = next_req.m_order_id;
    _order_item.m_state = order_state::PENDING_NEW;
    strat_man->trackOrder(next_req.m_order_id, m_strat_id);
}

void BaseStrategy::sendCancel(OrderItem& _order_item){
    next_req.clear();
    next_req.m_type = req_type::CANCEL;
    next_req.m_req_price = _order_item.m_order_price;
    next_req.m_order_quant = _order_item.m_order_quant;
    next_req.m_total_fills = _order_item.m_total_fills;
    next_req.m_order_id = _order_item.m_id;
    next_req.m_symbolId = sym_man->getSymbolID();
    mShmemManager->pushReq(next_req);

    _order_item.m_state = order_state::PENDING_CANCEL;
}

void BaseStrategy::processResp(const Response& _new_response){
    if(_new_response.m_type == resp_type::TRADE_CONFIRM){
        processRMFill(_new_response.m_side, _new_response.m_resp_price, _new_response.m_resp_quant);
    }
    gotResp(_new_response);
}

void BaseStrategy::processRMFill(const side& _side, const Price& _price, const Shares& _shares){
    mRMManager->subtractExposure(m_exposure);
    if(_side == side::BUY)
    {
        m_strat_position += _shares;
        m_exposure -= _price * _shares;
        LOG(INFO) << "TRADE|STRAT_ID=" << m_strat_id << "|SYMBOL=" << mSymIDManager->getTicker(sym_man->getSymbolID()) << 
           "|SIDE=BUY|PRICE=" << _price << "|QUANT=" << _shares << "|TIMESTAMP=" << sym_man->getCurrentTime();
    }
    else if(_side == side::SELL)
    {
        m_strat_position -= _shares;
        m_exposure += _price * _shares;
        LOG(INFO) << "TRADE|STRAT_ID=" << m_strat_id << "|SYMBOL=" << mSymIDManager->getTicker(sym_man->getSymbolID()) << 
           "|SIDE=SELL|PRICE=" << _price << "|QUANT=" << _shares << "|TIMESTAMP=" << sym_man->getCurrentTime();
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