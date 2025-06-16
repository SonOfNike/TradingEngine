#pragma once

#include "ShmemManager.h"
#include "RMManager.h"
#include "LogManager.h"
#include "ConfManager.h"
#include "SymbolManager.h"
#include "StrategyManager.h"
#include "../Utils/Response.h"
#include "../Utils/MDupdate.h"
#include "../Utils/SymbolIDManager.h"
#include <vector>

class TradingEngine {
    // singletons
    LogManager* mLogManager;
    RMManager* mRMManager;
    ConfManager* mConfManager;
    ShmemManager* mShmemManager;
    SymbolIDManager* mSymIDManager;

    MDupdate    currentMD;
    Response    currentResp;

    std::vector<StrategyManager> m_strat_managers;
    std::vector<SymbolManager> m_symbol_managers;

public:
    TradingEngine(){;}
    ~TradingEngine(){;}
    void startUp();
    void shutDown();
    void run();
    void processMD();
    void processResp();
};