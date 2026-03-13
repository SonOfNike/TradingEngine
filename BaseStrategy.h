#pragma once

// #include "BaseState.h"
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

enum class StratState {StartingState, WaitForTriggerState, OrdersOutState, CancelOpenState, CoveringState, FinishedState};

class BaseStrategy{
public:
    SymbolManager* sym_man;
    StrategyManager* strat_man;
    ShmemManager* mShmemManager;
    SymbolIDManager* mSymIDManager;
    RMManager* mRMManager;
    TimeManager* mTimeManager;

    Price m_exposure = 0;
    Price m_pnl = 0;

    Request next_req;
    LogItem next_log;

    OrderItem pcover_order;

    Shares m_strat_position = 0;

    SymbolId m_strat_id = 0;

    StratState current_state = StratState::StartingState;

    void onInit(SymbolManager* _sym_man, SymbolId _strat_id, StrategyManager* _strat_man){
        sym_man = _sym_man;
        strat_man = _strat_man;
        m_strat_id = _strat_id;
        mShmemManager = ShmemManager::getInstance();
        mRMManager = RMManager::getInstance();
        mSymIDManager = SymbolIDManager::getInstance();
        mTimeManager = TimeManager::getInstance();
    }

    void setState(StratState new_state){
        current_state = new_state;
    }

    void processResp(const Response& _new_response);

    void sendOrder(OrderItem& _order_item);

    void modOrder(OrderItem& _order_item);

    void sendCancel(OrderItem& _order_item);

    void setTimeout(const Timestamp& _time);

    void processRMFill(const side& _side, const Price& _price, const Shares& _shares);

    //passive cover
    void passiveCover();

    bool pcoverOrderCanceled();

    void cancelPCover();

    void sendPCover();

    MyOrderId getPCoverOrderID();
};


template <typename Strat>
class Strategy{
    Strat s;
public:
    void onInit(SymbolManager* _sym_man, SymbolId _strat_id, simdjson::dom::element _strat, StrategyManager* _strat_man){
        s.onInit(_sym_man, _strat_id, _strat, _strat_man);
    }

    void run(){
        switch (s.current_state){
            case StratState::StartingState:
                s.ready_to_start();
                break;
            case StratState::WaitForTriggerState:
                s.triggerCheck();
                break;
            case StratState::OrdersOutState:
                s.ordersOut();
                break;
            case StratState::CancelOpenState:
                s.cancelOpen();
                break;
            case StratState::CoveringState:
                s.covering();
                break;
            case StratState::FinishedState:
                s.finished_check();
                break;
        }
    }
    void gotPrint(){
        s.gotPrint();    
        run();
    }
    void gotQuote(){
        s.gotQuote();
        run();
    }
    void gotImbalance(){
        s.gotImbalance();    
        run();
    }
    void gotTimeout(){
        s.gotTimeout();
        run();
    }

    void processResp(const Response& _new_response){
        s.processResp(_new_response);
        s.gotResp(_new_response);
    }
};