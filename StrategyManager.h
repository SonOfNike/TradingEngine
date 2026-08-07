#pragma once

#include <vector>
#include <variant>
#include <unordered_map>
#include "../Utils/enums_typedef.h"
#include "../Utils/simdjson/simdjson.h"
#include "../Strategies/MeanReversion/MeanRevertStrategy.h"
#include "../Strategies/NYSEOpen/NYSEOpenStrategy.h"
#include "../Strategies/NYSEClose/NYSECloseStrategy.h"
#include "../Strategies/Momentum/MomentumStrategy.h"

class BaseStrategy;

using MyVariant = std::variant<Strategy<MeanRevertStrategy>, Strategy<NYSEOpenStrategy>, Strategy<NYSECloseStrategy>, Strategy<MomentumStrategy>>;

template<typename ConcreteStrategy>
struct StrategyCaller {
    static void gotQuote(void* instance){
        static_cast<Strategy<ConcreteStrategy>*>(instance)->gotQuote();
    }

    static void gotPrint(void* instance){
        static_cast<Strategy<ConcreteStrategy>*>(instance)->gotPrint();
    }

    static void gotImbalance(void* instance){
        static_cast<Strategy<ConcreteStrategy>*>(instance)->gotImbalance();
    }
};

class StrategyManager{
public:

    void OnInit(SymbolManager* _sym_man, simdjson::dom::element _symbol);

    void gotPrint();
    void gotQuote();
    void gotImbalance();
    void gotTimeout(const SymbolId& strat_id);

    void trackOrder(const MyOrderId& _order_id, const SymbolId& _strat_id, const int& index);

    void processResp(const Response& _new_response, const int& index);

private:
    using EventHandlerFunc = void(*)(void*);

    std::vector<MyVariant> m_strategies;

    SymbolId next_strat_id = 0;

    std::vector<SymbolId> m_order_to_strat;

    // std::unordered_map<MyOrderId , SymbolId> id_to_strat_id;
};