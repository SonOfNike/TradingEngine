#include "TradingEngine.h"
#include <iostream>
#include "../Utils/simdjson/simdjson.h"
#include "../ReqAlpacaAPI/client.h"

void TradingEngine::startUp(){
    // mLogManager = new LogManager();
    mRMManager = RMManager::getInstance();
    // mConfManager = new ConfManager();
    mSymIDManager = SymbolIDManager::getInstance();
    mShmemManager = ShmemManager::getInstance();

    mSymIDManager->startUp();
    mRMManager->startUp();
    mShmemManager->startUp();

    simdjson::dom::parser parser;

    simdjson::dom::element doc = parser.load("/home/git_repos/Utils/symbols.json");
    for(auto symbol : doc["symbols"])
    {
        m_symbol_managers.emplace_back(new SymbolManager);
        m_strat_managers.emplace_back(new StrategyManager);
    }

    doc = parser.load("/home/git_repos/FinvizData/output.json");
    
    simdjson::dom::object obj = doc["symbols"].get_object();

    std::cout << "Get open time" << std::endl;
    Timestamp open_time = doc["market_open_time"].get_uint64();

    auto env = alpaca::Environment();
    auto client = alpaca::Client(env);

    for(simdjson::dom::key_value_pair field : obj)
    {
        std::string s_copy(field.key);
        auto asset_response = client.getAsset(s_copy);
        
        auto asset = asset_response.second;

        SymbolId next_sym_id = mSymIDManager->getID(field.key);
        m_symbol_managers[next_sym_id]->OnInit(next_sym_id, open_time, asset.easy_to_borrow);
        m_strat_managers[next_sym_id]->OnInit(m_symbol_managers[next_sym_id],doc["symbols"][field.key]);
    }

    // mLogManager->startUp();
    // mConfManager->startUp();
}

void TradingEngine::shutDown(){
    // mLogManager->shutDown();
    // mConfManager->shutDown();
    mShmemManager->shutDown();
    mRMManager->shutDown();
    mSymIDManager->shutDown();

    // delete mLogManager;
    // delete mConfManager;
    delete mShmemManager;
    delete mRMManager;
    delete mSymIDManager;
}

void TradingEngine::run(){
    while(true){
        if(mShmemManager->gotResp()){
            DLOG(INFO) << "DEBUG|GOT_RESPONSE";
            processResp();
        }
        else if(mShmemManager->gotMD()){
            processMD();
        }
    }
}

void TradingEngine::processResp(){
    mShmemManager->getResp(currentResp);
    DLOG(INFO) << "DEBUG|RESPONSE_INFO|SYM_ID=" << currentResp.m_symbolId << "|SYMBOL=" << mSymIDManager->getTicker(currentResp.m_symbolId);
    m_strat_managers[currentResp.m_symbolId]->processResp(currentResp);
}

void TradingEngine::processMD(){
    mShmemManager->getMD(currentMD);
    if(currentMD.m_type == md_type::PRINT){
        m_symbol_managers[currentMD.m_symbolId]->gotPrint(currentMD.m_bid_price, currentMD.m_bid_quant, currentMD.m_timestamp);
        m_strat_managers[currentMD.m_symbolId]->gotPrint();
    }
    else if(currentMD.m_type == md_type::QUOTE){
        m_symbol_managers[currentMD.m_symbolId]->gotQuote(currentMD.m_bid_price, currentMD.m_bid_quant, currentMD.m_ask_price, currentMD.m_ask_quant, currentMD.m_timestamp);
        m_strat_managers[currentMD.m_symbolId]->gotQuote();
    }
}