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
    for(int i = 0; i < TRADE_WTHREADS; i++){
        std::vector<Timeout> vec;
        vec.reserve(512);

        heaps[i].min_heap = std::priority_queue<Timeout, std::vector<Timeout>, TimeoutComparator>(TimeoutComparator(), std::move(vec));
    }
}

void TimeManager::shutDown(){
    
}

void TimeManager::addTimeout(const Timestamp& time, const SymbolId& sym_id, const SymbolId& strat_id, const int& index){
    heaps[index].min_heap.push(Timeout(time, sym_id, strat_id));
}

bool TimeManager::gotTimeout(const Timestamp& _current_time, const int& index){
    if(heaps[index].min_heap.empty()) return false;
    
    if(_current_time >= heaps[index].min_heap.top().m_time)
        return true;

    return false;
}
    
void TimeManager::getTimeout(Timeout& next_timeout, const int& index){
    next_timeout = heaps[index].min_heap.top();

    heaps[index].min_heap.pop();
}