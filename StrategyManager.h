#pragma once

#include <vector>
#include <unordered_map>
#include "../Utils/enums_typedef.h"
#include "BaseStrategy.h"
#include "../Utils/simdjson/simdjson.h"
#include "../Strategies/MeanReversion/MeanRevertStrategy.h"

class StrategyManager{
public:

    void OnInit(SymbolManager* _sym_man, simdjson::dom::element _symbol){
        for(auto strat : _symbol["strats"])
        {
            std::string_view value = strat["strat_name"].get_string();
            if(value == "meanRevert"){
                m_strategies.emplace_back(new MeanRevertStrategy);
                m_strategies[next_strat_id]->onInit(_sym_man, next_strat_id, strat);
            }

            next_strat_id++;
        }
    }

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