#include "TradingEngine.h"
#include <iostream>

void TradingEngine::startUp(){
    // mLogManager = new LogManager();
    // mRMManager = new RMManager();
    // mConfManager = new ConfManager();
    mSymIDManager = SymbolIDManager::getInstance();
    mShmemManager = ShmemManager::getInstance();

    // mLogManager->startUp();
    // mRMManager->startUp();
    // mConfManager->startUp();
    mSymIDManager->startUp();
    mShmemManager->startUp();
}

void TradingEngine::shutDown(){
    // mLogManager->shutDown();
    // mRMManager->shutDown();
    // mConfManager->shutDown();
    mShmemManager->shutDown();
    mSymIDManager->shutDown();

    // delete mLogManager;
    // delete mRMManager;
    // delete mConfManager;
    delete mShmemManager;
    delete mSymIDManager;
}

void TradingEngine::run(){
    while(true){
        if(mShmemManager->gotResp()){
            processResp();
        }
        else if(mShmemManager->gotMD()){
            processMD();
        }
    }
}

void TradingEngine::processResp(){
    mShmemManager->getResp(currentResp);

}

void TradingEngine::processMD(){
    mShmemManager->getMD(currentMD);
    if(currentMD.m_type == md_type::PRINT){
        std::cout << "Received trade" << std::endl;
        std::cout << "Trade Price:" << currentMD.m_bid_price << std::endl;
        std::cout << "Trade Quant:" << currentMD.m_bid_quant << std::endl;
    }
    else if(currentMD.m_type == md_type::QUOTE){
        std::cout << "Received quote" << std::endl;
        std::cout << "Bid Price:" << currentMD.m_bid_price << std::endl;
        std::cout << "Ask Price:" << currentMD.m_ask_price << std::endl;
    }
}