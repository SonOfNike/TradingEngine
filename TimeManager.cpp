#include "TimeManager.h"
#include "../Utils/globals.h"
#include <iostream>

TimeManager* TimeManager::uniqueInstance = nullptr;

TimeManager* TimeManager::getInstance(){
    if(uniqueInstance == nullptr){
        uniqueInstance = new TimeManager();
    }
    return uniqueInstance;
}

void TimeManager::startUp(){
    std::vector<Timeout> vec;
    vec.reserve(1024);

    min_heap = std::priority_queue<Timeout, std::vector<Timeout>, TimeoutComparator>(TimeoutComparator(), std::move(vec));
}

void TimeManager::shutDown(){
    
}

void TimeManager::addTimeout(const Timestamp& time, const SymbolId& sym_id, const SymbolId& strat_id){
    min_heap.push(Timeout(time, sym_id, strat_id));
}

bool TimeManager::gotTimeout(const Timestamp& _current_time){
    if(min_heap.empty()) return false;
    
    if(_current_time >= min_heap.top().m_time)
        return true;

    return false;
}
    
void TimeManager::getTimeout(Timeout& next_timeout){
    next_timeout = min_heap.top();

    min_heap.pop();
}