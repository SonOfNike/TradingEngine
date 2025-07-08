#pragma once

#include <vector>
#include <unordered_map>
#include "../Utils/enums_typedef.h"
#include "../Utils/simdjson/simdjson.h"
#include "../Strategies/MeanReversion/MeanRevertStrategy.h"

class BaseStrategy;

class StrategyManager{
public:

    void OnInit(SymbolManager* _sym_man, simdjson::dom::element _symbol);

    void gotPrint();
    void gotQuote();
    void gotImbalance();

    void trackOrder(OrderId _order_id, SymbolId _strat_id);

    void processResp(const Response& _new_response);

private:
    std::vector<BaseStrategy*> m_strategies;
    SymbolId next_strat_id = 0;

    std::unordered_map<OrderId , SymbolId> id_to_strat_id;
};