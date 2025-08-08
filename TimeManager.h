#pragma once

#include "../Utils/enums_typedef.h"
#include "../Utils/Timeout.h"
#include <queue>
#include <vector>
#include <functional>

class TimeManager {
private:
    static TimeManager* uniqueInstance;
    TimeManager(){;}

    std::priority_queue<Timeout, std::vector<Timeout>, TimeoutComparator> min_heap;

public:
    static TimeManager* getInstance();
    void startUp();
    void shutDown();
    void addTimeout(const Timestamp& time, const SymbolId& sym_id, const SymbolId& strat_id);
    bool gotTimeout(const Timestamp& _current_time);
    void getTimeout(Timeout& next_timeout);
};