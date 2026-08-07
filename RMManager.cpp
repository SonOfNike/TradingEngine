#include "RMManager.h"
#include "../Utils/simdjson/simdjson.h"
#include "../Utils/globals.h"

RMManager* RMManager::uniqueInstance = nullptr;

RMManager* RMManager::getInstance(){
    if(uniqueInstance == nullptr){
        uniqueInstance = new RMManager();
    }
    return uniqueInstance;
}

void RMManager::startUp(){

    simdjson::dom::parser parser;

    simdjson::dom::element doc = parser.load("/home/git_repos/Confs/RM_values.json");
    
    m_global_exposure_limit = doc["global_expo"].get_int64() * DOLLAR;
    m_global_pnl_limit = doc["global_pnl"].get_int64() * DOLLAR;
    m_sym_pnl_limit = doc["sym_pnl"].get_int64() * DOLLAR;
}

void RMManager::shutDown(){
    
}

void RMManager::subtractExposure(const Price& _exposure){
    m_global_exposure.fetch_sub(abs(_exposure), std::memory_order_relaxed);
}

void RMManager::addExposure(const Price& _exposure){
    m_global_exposure.fetch_add(abs(_exposure), std::memory_order_relaxed);
}

void RMManager::subtractPNL(const Price& _pnl){
    m_global_pnl.fetch_sub(_pnl, std::memory_order_relaxed);
}

void RMManager::addPNL(const Price& _pnl){
    m_global_pnl.fetch_add(_pnl, std::memory_order_relaxed);
}

bool RMManager::exposureLimitBreached(){
    if(m_global_exposure.load(std::memory_order_acquire) >= m_global_exposure_limit) return true;

    return false;
}

bool RMManager::pnlLimitBreached(){
    if(m_global_pnl.load(std::memory_order_acquire) >= m_global_pnl_limit) return true;

    return false;
}

Price RMManager::getSymPnLLimit(){
    return m_sym_pnl_limit;
}