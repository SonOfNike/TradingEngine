#pragma once

#include "ShmemManager.h"
#include "RMManager.h"
#include "LogManager.h"
#include "ConfManager.h"
#include "../Utils/Response.h"
#include "../Utils/MDupdate.h"

class TradingEngine {
    // singletons
    LogManager* mLogManager;
    RMManager* mRMManager;
    ConfManager* mConfManager;
    ShmemManager* mShmemManager;
    MDupdate    currentMD;
    Response    currentResp;

public:
    TradingEngine(){;}
    ~TradingEngine(){;}
    void startUp();
    void shutDown();
    void run();
    void processMD();
    void processResp();
};