#include "StrategyManager.h"
#include "BaseStrategy.h"
#include "../Strategies/ORBO/ORBOStrategy.h"
#include "../Strategies/VWAPReversion/VWAPRevertStrategy.h"

void StrategyManager::OnInit(SymbolManager* _sym_man, simdjson::dom::element _symbol){
    for(auto strat : _symbol["strats"])
    {
        std::string_view value = strat["strat_name"].get_string();
        if(value == "meanRevert"){
            m_strategies.emplace_back(new MeanRevertStrategy);
            m_strategies[next_strat_id]->onInit(_sym_man, next_strat_id, strat, this);
        }
        else if(value == "ORBO"){
            m_strategies.emplace_back(new ORBOStrategy);
            m_strategies[next_strat_id]->onInit(_sym_man, next_strat_id, strat, this);
        }
        else if(value == "VWAPRevert"){
            m_strategies.emplace_back(new VWAPRevertStrategy);
            m_strategies[next_strat_id]->onInit(_sym_man, next_strat_id, strat, this);
        }

        next_strat_id++;
    }
}

void StrategyManager::gotPrint(){
    for(auto strat : m_strategies){
        strat->gotPrint();
    }
}
void StrategyManager::gotQuote(){
    for(auto strat : m_strategies){
        strat->gotQuote();
    }
}
void StrategyManager::gotImbalance(){
    for(auto strat : m_strategies){
        strat->gotImbalance();
    }
}

void StrategyManager::gotTimeout(const SymbolId& strat_id){
    m_strategies[strat_id]->gotTimeout();
}

void StrategyManager::trackOrder(const OrderId& _order_id, const SymbolId& _strat_id){
    id_to_strat_id[_order_id] = _strat_id;
}

void StrategyManager::processResp(const Response& _new_response){
    m_strategies[id_to_strat_id[_new_response.m_order_id]]->processResp(_new_response);
}
