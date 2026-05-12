#include "StrategyManager.h"
#include "BaseStrategy.h"

void StrategyManager::OnInit(SymbolManager* _sym_man, simdjson::dom::element _symbol){
    for(auto strat : _symbol["strats"])
    {
        std::string_view value = strat["strat_name"].get_string();
        if(value == "meanRevert"){
            m_strategies.emplace_back(Strategy<MeanRevertStrategy>());
            std::visit([this, &_sym_man, &strat](auto& obj) {
                obj.onInit(_sym_man, next_strat_id, strat, this, false);
            }, m_strategies[next_strat_id]);
        }
        else if(value == "NYSEOpen"){
            m_strategies.emplace_back(Strategy<NYSEOpenStrategy>());
            std::visit([this, &_sym_man, &strat](auto& obj) {
                obj.onInit(_sym_man, next_strat_id, strat, this, true);
            }, m_strategies[next_strat_id]);
        }
        else if(value == "NYSEClose"){
            m_strategies.emplace_back(Strategy<NYSECloseStrategy>());
            std::visit([this, &_sym_man, &strat](auto& obj) {
                obj.onInit(_sym_man, next_strat_id, strat, this, false);
            }, m_strategies[next_strat_id]);
        }
        // else if(value == "ORBO"){
        //     m_strategies.emplace_back(new ORBOStrategy);
        //     m_strategies[next_strat_id]->onInit(_sym_man, next_strat_id, strat, this);
        // }
        // else if(value == "VWAPRevert"){
        //     m_strategies.emplace_back(new VWAPRevertStrategy);
        //     m_strategies[next_strat_id]->onInit(_sym_man, next_strat_id, strat, this);
        // }

        next_strat_id++;
    }
}

void StrategyManager::gotPrint(){
    for(auto& strat : m_strategies){
        std::visit([](auto& obj) {
            obj.gotPrint();
        }, strat);
    }
}
void StrategyManager::gotQuote(){
    for(auto& strat : m_strategies){
        std::visit([](auto& obj) {
            obj.gotQuote();
        }, strat);
    }
}
void StrategyManager::gotImbalance(){
    for(auto& strat : m_strategies){
        std::visit([](auto& obj) {
            obj.gotImbalance();
        }, strat);
    }
}

void StrategyManager::gotTimeout(const SymbolId& strat_id){
    std::visit([](auto& obj) {
        obj.gotTimeout();
    }, m_strategies[strat_id]);
}

void StrategyManager::trackOrder(const MyOrderId& _order_id, const SymbolId& _strat_id){
    id_to_strat_id[_order_id] = _strat_id;
}

void StrategyManager::processResp(const Response& _new_response){
    std::visit([&_new_response](auto& obj) {
        obj.processResp(_new_response);
    }, m_strategies[id_to_strat_id[_new_response.m_order_id]]);
}
