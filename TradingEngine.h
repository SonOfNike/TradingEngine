#pragma once

#include "ShmemManager.h"
#include "RMManager.h"
#include "SymbolManager.h"
#include "TimeManager.h"
#include "StrategyManager.h"
#include "../Utils/Response.h"
#include "../Utils/MDupdate.h"
#include "../Utils/Timeout.h"
#include "../Utils/SymbolIDManager.h"
#include <vector>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "../Utils/httplib.h"

struct alignas(64) ThreadContext{
    MDupdate    currentMD;
    Response    currentResp;
    Timeout     currentTimeout;
    Timestamp   currentTime = 0;
};

struct alignas(64) SymbolStrat{
    SymbolManager m_sym_man;
    StrategyManager m_strat_man;
};

class TradingEngine {
    // singletons
    RMManager* mRMManager;
    ShmemManager* mShmemManager;
    SymbolIDManager* mSymIDManager;
    TimeManager* mTimeManager;

    // MDupdate    currentMD[5];
    // Response    currentResp[5];
    // Timeout     currentTimeout[5];
    // Timestamp   currentTime[5] = {0,0,0,0,0};

    std::array<ThreadContext, TRADE_WTHREADS> mThreadContexts;

    // std::vector<StrategyManager> m_strat_managers;
    // std::vector<SymbolManager> m_symbol_managers;

    std::vector<SymbolStrat> m_symbol_strats;

public:
    TradingEngine(){;}
    ~TradingEngine(){;}
    void startUp();
    void shutDown();
    void run(const int& index);
    void processMD(const int& index);
    void processResp(const int& index);
    void processTimeout(const int& index);
};