#pragma once

#include "BaseState.h"
#include "SymbolManager.h"
#include "ShmemManager.h"
#include "TimeManager.h"
#include "RMManager.h"
#include "../Utils/simdjson/simdjson.h"
#include "../Utils/OrderItem.h"
#include "../Utils/Request.h"
#include "../Utils/LogItem.h"
#include "../Utils/SymbolIDManager.h"

class StrategyManager;

class BaseStrategy{
public:
    BaseState* current_state;
    SymbolManager* sym_man;
    StrategyManager* strat_man;
    ShmemManager* mShmemManager;
    SymbolIDManager* mSymIDManager;
    RMManager* mRMManager;
    TimeManager* mTimeManager;

    SymbolId m_strat_id = 0;

    Shares m_strat_position = 0;
    Price m_exposure = 0;
    Price m_pnl = 0;

    Request next_req;
    LogItem next_log;

    void onInit(SymbolManager* _sym_man, SymbolId _strat_id, StrategyManager* _strat_man){
        sym_man = _sym_man;
        strat_man = _strat_man;
        m_strat_id = _strat_id;
        mShmemManager = ShmemManager::getInstance();
        mRMManager = RMManager::getInstance();
        mSymIDManager = SymbolIDManager::getInstance();
        mTimeManager = TimeManager::getInstance();
    }

    virtual void run(){current_state->run();}
    virtual void gotPrint(){current_state->gotPrint();}
    virtual void gotQuote(){current_state->gotQuote();}
    virtual void gotImbalance(){current_state->gotImbalance();}
    virtual void gotTimeout(){current_state->gotTimeout();}

    virtual void gotResp(const Response& _new_response) = 0;

    virtual void logParams() = 0;
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

    void setTimeout(const Timestamp& _time);

    void processRMFill(const side& _side, const Price& _price, const Shares& _shares);
};