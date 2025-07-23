#include "TradingEngine.h"
#include <iostream>
#include "../Utils/simdjson/simdjson.h"
#include "../ReqAlpacaAPI/client.h"

void TradingEngine::startUp(){
    // mLogManager = new LogManager();
    // mRMManager = new RMManager();
    // mConfManager = new ConfManager();
    mSymIDManager = SymbolIDManager::getInstance();
    mShmemManager = ShmemManager::getInstance();

    mSymIDManager->startUp();

    simdjson::dom::parser parser;

    simdjson::dom::element doc = parser.load("/home/git_repos/Utils/symbols.json");
    for(auto symbol : doc["symbols"])
    {
        m_symbol_managers.emplace_back(new SymbolManager);
        m_strat_managers.emplace_back(new StrategyManager);
    }

    doc = parser.load("/home/git_repos/Strategies/param_list.json");
    
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
        m_strat_managers[next_sym_id]->OnInit(m_symbol_managers[next_sym_id],doc["symbols"][field.key]);
        m_symbol_managers[next_sym_id]->OnInit(next_sym_id, open_time, asset.easy_to_borrow);
    }

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
    m_strat_managers[currentResp.m_symbolId]->processResp(currentResp);
}

void TradingEngine::processMD(){
    mShmemManager->getMD(currentMD);
    if(currentMD.m_type == md_type::PRINT){
        std::cout << "Received trade" << std::endl;
        std::cout << "Trade Price:" << currentMD.m_bid_price << std::endl;
        std::cout << "Trade Quant:" << currentMD.m_bid_quant << std::endl;
        m_symbol_managers[currentMD.m_symbolId]->gotPrint(currentMD.m_bid_price, currentMD.m_bid_quant, currentMD.m_timestamp);
        m_strat_managers[currentMD.m_symbolId]->gotPrint();
    }
    else if(currentMD.m_type == md_type::QUOTE){
        std::cout << "Received quote" << std::endl;
        std::cout << "Bid Price:" << currentMD.m_bid_price << std::endl;
        std::cout << "Ask Price:" << currentMD.m_ask_price << std::endl;
        m_symbol_managers[currentMD.m_symbolId]->gotQuote(currentMD.m_bid_price, currentMD.m_bid_quant, currentMD.m_ask_price, currentMD.m_ask_quant, currentMD.m_timestamp);
        m_strat_managers[currentMD.m_symbolId]->gotQuote();
    }
}