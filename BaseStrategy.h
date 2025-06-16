#pragma once

#include "BaseState.h"

class BaseStrategy{
    BaseState* current_state;
    SymbolManager* sym_man;

public:
    void run(){current_state->run();}
    void gotPrint(){current_state->gotPrint();}
    void gotQuote(){current_state->gotQuote();}
    void gotImbalance(){current_state->gotImbalance();}

    void processResp(const Response& _new_response){;}
};