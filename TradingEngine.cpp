#include "TradingEngine.h"

void TradingEngine::startUp(){
    // mLogManager = new LogManager();
    // mRMManager = new RMManager();
    // mConfManager = new ConfManager();
    mShmemManager = ShmemManager::getInstance();

    // mLogManager->startUp();
    // mRMManager->startUp();
    // mConfManager->startUp();
    mShmemManager->startUp();
}

void TradingEngine::shutDown(){
    // mLogManager->shutDown();
    // mRMManager->shutDown();
    // mConfManager->shutDown();
    mShmemManager->shutDown();

    // delete mLogManager;
    // delete mRMManager;
    // delete mConfManager;
    delete mShmemManager;
}

void TradingEngine::run(){
    ;
}