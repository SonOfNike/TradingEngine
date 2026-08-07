#pragma once

#include "../Utils/enums_typedef.h"
#include "../Utils/Timeout.h"
#include "../Utils/globals.h"
#include <queue>
#include <vector>
#include <functional>

struct alignas(64) QueueContext{
    std::priority_queue<Timeout, std::vector<Timeout>, TimeoutComparator> min_heap;
};

class TimeManager {
private:
    static TimeManager* uniqueInstance;
    TimeManager(){;}

    // std::priority_queue<Timeout, std::vector<Timeout>, TimeoutComparator> min_heap[TRADE_WTHREADS];
    QueueContext heaps[TRADE_WTHREADS];

public:
    static TimeManager* getInstance();
    void startUp();
    void shutDown();
    void addTimeout(const Timestamp& time, const SymbolId& sym_id, const SymbolId& strat_id, const int& index);
    bool gotTimeout(const Timestamp& _current_time, const int& index);
    void getTimeout(Timeout& next_timeout, const int& index);
};