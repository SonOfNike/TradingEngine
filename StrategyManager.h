#pragma once

#include <vector>
#include <variant>
#include <unordered_map>
#include "../Utils/enums_typedef.h"
#include "../Utils/simdjson/simdjson.h"
#include "../Strategies/MeanReversion/MeanRevertStrategy.h"
#include "../Strategies/NYSEOpen/NYSEOpenStrategy.h"
#include "../Strategies/NYSEClose/NYSECloseStrategy.h"

class BaseStrategy;

using MyVariant = std::variant<Strategy<MeanRevertStrategy>, Strategy<NYSEOpenStrategy>, Strategy<NYSECloseStrategy>>;

class StrategyManager{
public:

    void OnInit(SymbolManager* _sym_man, simdjson::dom::element _symbol);

    void gotPrint();
    void gotQuote();
    void gotImbalance();
    void gotTimeout(const SymbolId& strat_id);

    void trackOrder(const MyOrderId& _order_id, const SymbolId& _strat_id);

    void processResp(const Response& _new_response);

private:
    std::vector<MyVariant> m_strategies;
    SymbolId next_strat_id = 0;

    std::unordered_map<MyOrderId , SymbolId> id_to_strat_id;
};