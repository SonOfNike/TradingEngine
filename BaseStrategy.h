#pragma once

#include "BaseState.h"
#include "SymbolManager.h"
#include "../Utils/simdjson/simdjson.h"

class BaseStrategy{
public:
    BaseState* current_state;
    SymbolManager* sym_man;
    SymbolId m_strat_id = 0;

    Shares m_strat_position = 0;

    void onInit(SymbolManager* _sym_man, SymbolId _strat_id){
        sym_man = _sym_man;
        m_strat_id = _strat_id;
    }

    void run(){current_state->run();}
    void gotPrint(){current_state->gotPrint();}
    void gotQuote(){current_state->gotQuote();}
    void gotImbalance(){current_state->gotImbalance();}

    virtual void ready_to_start() = 0;
    virtual void onInit(SymbolManager* _sym_man, SymbolId _strat_id, simdjson::dom::element _strat) = 0;

    void setState(BaseState* new_state){
        current_state = new_state;
    }

    void processResp(const Response& _new_response){;}
};