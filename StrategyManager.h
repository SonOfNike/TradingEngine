#pragma once

#include <vector>
#include <unordered_map>
#include "../Utils/enums_typedef.h"
#include "BaseStrategy.h"

class StrategyManager{
public:

    void gotPrint(){
        for(auto strat : m_strategies){
            strat->gotPrint();
        }
    }
    void gotQuote(){
        for(auto strat : m_strategies){
            strat->gotQuote();
        }
    }
    void gotImbalance(){
        for(auto strat : m_strategies){
            strat->gotImbalance();
        }
    }

    void processResp(const Response& _new_response){
        m_strategies[id_to_strat_id[_new_response.m_order_id]]->processResp(_new_response);
    }

private:
    std::vector<BaseStrategy*> m_strategies;
    SymbolId next_strat_id = 0;

    std::unordered_map<OrderId , SymbolId> id_to_strat_id;
};