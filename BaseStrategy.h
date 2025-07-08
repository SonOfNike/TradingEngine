#pragma once

#include "BaseState.h"
#include "SymbolManager.h"
#include "ShmemManager.h"
#include "../Utils/simdjson/simdjson.h"
#include "../Utils/OrderItem.h"
#include "../Utils/Request.h"

class StrategyManager;

class BaseStrategy{
public:
    BaseState* current_state;
    SymbolManager* sym_man;
    StrategyManager* strat_man;
    ShmemManager* mShmemManager;
    SymbolId m_strat_id = 0;

    Shares m_strat_position = 0;

    Request next_req;

    void onInit(SymbolManager* _sym_man, SymbolId _strat_id, StrategyManager* _strat_man){
        sym_man = _sym_man;
        strat_man = _strat_man;
        m_strat_id = _strat_id;
        mShmemManager = ShmemManager::getInstance();
    }

    virtual void run(){current_state->run();}
    virtual void gotPrint(){current_state->gotPrint();}
    virtual void gotQuote(){current_state->gotQuote();}
    virtual void gotImbalance(){current_state->gotImbalance();}

    virtual void gotResp(const Response& _new_response) = 0;

    virtual void ready_to_start() = 0;
    virtual void triggerCheck() = 0;
    virtual void cancelOpen() = 0;
    virtual void ordersOut() = 0;
    virtual void covering() = 0;
    virtual void onInit(SymbolManager* _sym_man, SymbolId _strat_id, simdjson::dom::element _strat, StrategyManager* _strat_man) = 0;

    void setState(BaseState* new_state){
        current_state = new_state;
    }

    void processResp(const Response& _new_response);

    void sendOrder(OrderItem& _order_item);

    void sendCancel(OrderItem& _order_item);
};