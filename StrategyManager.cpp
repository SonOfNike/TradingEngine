#include "StrategyManager.h"
#include "BaseStrategy.h"

void StrategyManager::OnInit(SymbolManager* _sym_man, simdjson::dom::element _symbol){
    m_order_to_strat.resize(256, -1);

    for(auto strat : _symbol["strats"])
    {
        std::string_view value = strat["strat_name"].get_string();
        if(value == "meanRevert"){
            auto& new_strat = m_strategies.emplace_back(Strategy<MeanRevertStrategy>());

            auto* instance_ptr = std::get_if<Strategy<MeanRevertStrategy>>(&new_strat);

            instance_ptr->onInit(_sym_man, next_strat_id, strat, this, false);


            // m_strategies.emplace_back(Strategy<MeanRevertStrategy>());
            // std::visit([this, &_sym_man, &strat](auto& obj) {
            //     obj.onInit(_sym_man, next_strat_id, strat, this, false);
            // }, m_strategies[next_strat_id]);
        }
        else if(value == "NYSEOpen"){
            auto& new_strat = m_strategies.emplace_back(Strategy<NYSEOpenStrategy>());

            auto* instance_ptr = std::get_if<Strategy<NYSEOpenStrategy>>(&new_strat);

            instance_ptr->onInit(_sym_man, next_strat_id, strat, this, false);

            // m_strategies.emplace_back(Strategy<NYSEOpenStrategy>());
            // std::visit([this, &_sym_man, &strat](auto& obj) {
            //     obj.onInit(_sym_man, next_strat_id, strat, this, true);
            // }, m_strategies[next_strat_id]);
        }
        else if(value == "NYSEClose"){
            auto& new_strat = m_strategies.emplace_back(Strategy<NYSECloseStrategy>());

            auto* instance_ptr = std::get_if<Strategy<NYSECloseStrategy>>(&new_strat);

            instance_ptr->onInit(_sym_man, next_strat_id, strat, this, false);


            // m_strategies.emplace_back(Strategy<NYSECloseStrategy>());
            // std::visit([this, &_sym_man, &strat](auto& obj) {
            //     obj.onInit(_sym_man, next_strat_id, strat, this, true);
            // }, m_strategies[next_strat_id]);
        }
        else if(value == "Momentum"){
            auto& new_strat = m_strategies.emplace_back(Strategy<MomentumStrategy>());

            auto* instance_ptr = std::get_if<Strategy<MomentumStrategy>>(&new_strat);

            instance_ptr->onInit(_sym_man, next_strat_id, strat, this, false);

            // m_strategies.emplace_back(Strategy<MomentumStrategy>());
            // std::visit([this, &_sym_man, &strat](auto& obj) {
            //     obj.onInit(_sym_man, next_strat_id, strat, this, true);
            // }, m_strategies[next_strat_id]);
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
    // size_t count = m_print_instances.size();
    // for(size_t i = 0; i < count; ++i){
    //     m_print_handlers[i](m_print_instances[i]);
    // }

    for(auto& strat : m_strategies){
        std::visit([](auto& obj) {
            obj.gotPrint();
        }, strat);
    }
}
void StrategyManager::gotQuote(){
    // size_t count = m_quote_instances.size();
    // for(size_t i = 0; i < count; ++i){
    //     m_quote_handlers[i](m_quote_instances[i]);
    // }

    for(auto& strat : m_strategies){
        std::visit([](auto& obj) {
            obj.gotQuote();
        }, strat);
    }
}
void StrategyManager::gotImbalance(){
    // size_t count = m_imbalance_instances.size();
    // for(size_t i = 0; i < count; ++i){
    //     m_imbalance_handlers[i](m_imbalance_instances[i]);
    // }

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

void StrategyManager::trackOrder(const MyOrderId& _order_id, const SymbolId& _strat_id, const int& index){
    MyOrderId cur_id = _order_id - (10000*index);
    if(cur_id >= m_order_to_strat.size()){
        m_order_to_strat.resize(cur_id +1024, -1);
    }
    m_order_to_strat[cur_id] = _strat_id;

    // id_to_strat_id[_order_id] = _strat_id;
}

void StrategyManager::processResp(const Response& _new_response, const int& index){
    SymbolId strat_id = m_order_to_strat[_new_response.m_order_id - (10000*index)];
    if(strat_id != (SymbolId)-1){
        std::visit([&_new_response](auto& obj) {
            obj.processResp(_new_response);
        }, m_strategies[strat_id]);
    }

    // std::visit([&_new_response](auto& obj) {
    //     obj.processResp(_new_response);
    // }, m_strategies[id_to_strat_id[_new_response.m_order_id]]);
}
