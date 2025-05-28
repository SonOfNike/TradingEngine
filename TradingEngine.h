#pragma once

#include "ShmemManager.h"
#include "RMManager.h"
#include "LogManager.h"
#include "ConfManager.h"

class TradingEngine {
    // singletons
    LogManager* mLogManager;
    RMManager* mRMManager;
    ConfManager* mConfManager;
    ShmemManager* mShmemManager;

public:
    TradingEngine(){;}
    ~TradingEngine(){;}
    void startUp();
    void shutDown();
    void run();
};