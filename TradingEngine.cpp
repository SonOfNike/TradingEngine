#include "TradingEngine.h"
#include <iostream>
#include "../Utils/simdjson/simdjson.h"

void TradingEngine::startUp(){
    // mLogManager = new LogManager();
    mRMManager = RMManager::getInstance();
    // mConfManager = new ConfManager();
    mSymIDManager = SymbolIDManager::getInstance();
    mShmemManager = ShmemManager::getInstance();
    mTimeManager = TimeManager::getInstance();

    mSymIDManager->startUp();
    mRMManager->startUp();
    mTimeManager->startUp();

    simdjson::dom::parser parser;

    simdjson::dom::element doc = parser.load("/home/git_repos/Confs/LIVE_auth.json");
    
    api_key_id_ = std::string(doc["key"].get_string().value());

    api_secret_key_ = std::string(doc["secret"].get_string().value());

    doc = parser.load("/home/git_repos/Utils/symbols.json");
    for(auto symbol : doc["symbols"])
    {
        m_symbol_managers.emplace_back(new SymbolManager);
        m_strat_managers.emplace_back(new StrategyManager);
    }

    doc = parser.load("/home/git_repos/FinvizData/output.json");
    
    simdjson::dom::object obj = doc["symbols"].get_object();

    std::cout << "Get open time" << std::endl;
    Timestamp open_time = doc["market_open_time"].get_uint64();

    for(simdjson::dom::key_value_pair field : obj)
    {
        std::string s_copy(field.key);

        bool easy_to_borrow = getEasyToBorrow(s_copy);

        SymbolId next_sym_id = mSymIDManager->getID(field.key);
        m_symbol_managers[next_sym_id]->OnInit(next_sym_id, open_time, easy_to_borrow, mShmemManager);
        m_strat_managers[next_sym_id]->OnInit(m_symbol_managers[next_sym_id],doc["symbols"][field.key]);

        // if(easy_to_borrow)
        //     std::cout << field.key << " easy to borrow" << std::endl;
        // else
        //     std::cout << field.key << " hard to borrow" << std::endl;
    }

    mShmemManager->startUp();

    // mLogManager->startUp();
    // mConfManager->startUp();
}

void TradingEngine::shutDown(){
    // mLogManager->shutDown();
    // mConfManager->shutDown();
    mShmemManager->shutDown();
    mRMManager->shutDown();
    mSymIDManager->shutDown();
    mTimeManager->shutDown();

    // delete mLogManager;
    // delete mConfManager;
    delete mShmemManager;
    delete mRMManager;
    delete mSymIDManager;
    delete mTimeManager;
}

void TradingEngine::run(){
    while(true){
        if(mShmemManager->gotResp()){
            processResp();
        }
        else if(mTimeManager->gotTimeout(currentTime)){
            processTimeout();
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
        m_symbol_managers[currentMD.m_symbolId]->gotPrint(currentMD.m_bid_price, currentMD.m_bid_quant, currentMD.m_timestamp);
        m_strat_managers[currentMD.m_symbolId]->gotPrint();
    }
    else if(currentMD.m_type == md_type::QUOTE){
        m_symbol_managers[currentMD.m_symbolId]->gotQuote(currentMD.m_bid_price, currentMD.m_bid_quant, currentMD.m_ask_price, currentMD.m_ask_quant, currentMD.m_timestamp);
        // m_strat_managers[currentMD.m_symbolId]->gotQuote();
    }
    currentTime = currentMD.m_timestamp;
}

void TradingEngine::processTimeout(){
    mTimeManager->getTimeout(currentTimeout);
    m_symbol_managers[currentTimeout.m_sym_id]->setCurrentTime(currentTime);
    m_strat_managers[currentTimeout.m_sym_id]->gotTimeout(currentTimeout.m_strat_id);
}

bool TradingEngine::getEasyToBorrow(const std::string& symbol) {
  auto url = "/v2/assets/" + symbol;

  httplib::SSLClient client("api.alpaca.markets");
  
  auto resp = client.Get(url.c_str(), headers());
  if (!resp) {
    return false;
  }

  if (resp->status != 200) {
    return false;
  }

  simdjson::padded_string padded_json_string(resp->body);

  bool easy_to_borrow = false;

  simdjson::dom::parser parser;

  simdjson::dom::object obj = parser.parse(resp->body);

  for(const auto& key_value : obj) {
      if(key_value.key == "easy_to_borrow"){
          easy_to_borrow = key_value.value;
          break;
      }
  }

  return easy_to_borrow;
}

httplib::Headers TradingEngine::headers() {
    return {
        {"APCA-API-KEY-ID", api_key_id_},
        {"APCA-API-SECRET-KEY", api_secret_key_},
    };
}