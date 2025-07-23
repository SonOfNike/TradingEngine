#include "StrategyManager.h"
#include "BaseStrategy.h"

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
        if(_new_response.m_side == side::BUY)
        {
            m_strat_position += _new_response.m_resp_quant;
        }
        else if(_new_response.m_side == side::SELL)
        {
            m_strat_position -= _new_response.m_resp_quant;
        }
    }
    gotResp(_new_response);
}