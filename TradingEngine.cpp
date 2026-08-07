#include "TradingEngine.h"
#include <iostream>
#include "../Utils/simdjson/simdjson.h"

void TradingEngine::startUp(){
    mRMManager = RMManager::getInstance();
    mSymIDManager = SymbolIDManager::getInstance();
    mShmemManager = ShmemManager::getInstance();
    mTimeManager = TimeManager::getInstance();

    mSymIDManager->startUp();
    mRMManager->startUp();
    mTimeManager->startUp();

    simdjson::dom::parser parser;

    simdjson::dom::element doc = parser.load("/home/git_repos/Utils/symbols.json");

    for(auto symbol : doc["symbols"])
    {
        m_symbol_strats.emplace_back(SymbolStrat());
    }

    // IB
    doc = parser.load("/home/git_repos/FinvizData/ib_output.json");
    
    simdjson::dom::object obj = doc["symbols"].get_object();

    std::cout << "Get open time" << std::endl;
    Timestamp open_time = doc["market_open_time"].get_uint64();

    for(simdjson::dom::key_value_pair field : obj)
    {
        std::string s_copy(field.key);

        SymbolId next_sym_id = mSymIDManager->getID(field.key);

        m_symbol_strats[next_sym_id].m_sym_man.OnInit(next_sym_id, open_time, doc["symbols"][field.key]["etb"].get_bool(), mShmemManager);
        m_symbol_strats[next_sym_id].m_strat_man.OnInit(&m_symbol_strats[next_sym_id].m_sym_man,doc["symbols"][field.key]);
    }

    std::cout << "List Loaded" << std::endl;

    mShmemManager->startUp();
}

void TradingEngine::shutDown(){
    mShmemManager->shutDown();
    mRMManager->shutDown();
    mSymIDManager->shutDown();
    mTimeManager->shutDown();

    delete mShmemManager;
    delete mRMManager;
    delete mSymIDManager;
    delete mTimeManager;
}

void TradingEngine::run(const int& index){
    while(true){

        if(mShmemManager->gotResp(index)) [[unlikely]]{
            processResp(index);
        }
        else if(mTimeManager->gotTimeout(mThreadContexts[index].currentTime, index))[[unlikely]]{
            processTimeout(index);
        }
        else if(mShmemManager->gotMD(index))[[likely]]{
            processMD(index);
        }

    }
}

void TradingEngine::processResp(const int& index){
    mShmemManager->getResp(mThreadContexts[index].currentResp, index);
    m_symbol_strats[mThreadContexts[index].currentResp.m_symbolId].m_strat_man.processResp(mThreadContexts[index].currentResp, index);
}

void TradingEngine::processMD(const int& index){
    auto& ctx = mThreadContexts[index];
    mShmemManager->getMD(ctx.currentMD, index);

    const auto& md = ctx.currentMD;
    auto& symbol = m_symbol_strats[md.m_symbolId].m_sym_man;
    auto& strategy = m_symbol_strats[md.m_symbolId].m_strat_man;

    switch(md.m_type){
        case md_type::QUOTE:
            symbol.gotQuote(md);
            strategy.gotQuote();
            break;
        case md_type::PRINT:
            symbol.gotPrint(md);
            strategy.gotPrint();
            break;
        case md_type::NYSEOPEN:
            symbol.gotNYSEOpen(md);
            strategy.gotPrint();
            break;
        case md_type::NASDOPEN:
            symbol.gotNASDOpen(md);
            strategy.gotPrint();
            break;
        case md_type::IMBALANCE:
            symbol.gotImbalance(md);
            strategy.gotImbalance();
            break;
        case md_type::SIGIMB:
            symbol.gotSigImbalance(md);
            strategy.gotImbalance();
            break;
    }

    mThreadContexts[index].currentTime = mThreadContexts[index].currentMD.m_timestamp;
}

void TradingEngine::processTimeout(const int& index){
    auto& curTimeout = mThreadContexts[index].currentTimeout;

    mTimeManager->getTimeout(curTimeout, index);
    m_symbol_strats[curTimeout.m_sym_id].m_strat_man.gotTimeout(curTimeout.m_strat_id);
}