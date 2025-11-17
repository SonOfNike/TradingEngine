#pragma once

#include "ShmemManager.h"
#include "RMManager.h"
#include "LogManager.h"
#include "ConfManager.h"
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

class TradingEngine {
    // singletons
    LogManager* mLogManager;
    RMManager* mRMManager;
    ConfManager* mConfManager;
    ShmemManager* mShmemManager;
    SymbolIDManager* mSymIDManager;
    TimeManager* mTimeManager;

    MDupdate    currentMD;
    Response    currentResp;
    Timeout     currentTimeout;
    Timestamp   currentTime = 0;

    std::string api_key_id_;
    std::string api_secret_key_;

    std::vector<StrategyManager*> m_strat_managers;
    std::vector<SymbolManager*> m_symbol_managers;

public:
    TradingEngine(){;}
    ~TradingEngine(){;}
    void startUp();
    void shutDown();
    void run();
    void processMD();
    void processResp();
    void processOrderError();
    void processTimeout();
    httplib::Headers headers();
    bool getEasyToBorrow(const std::string& symbol);
};