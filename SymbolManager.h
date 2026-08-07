#pragma once

#include "../Utils/enums_typedef.h"
#include "ShmemManager.h"
#include "../Utils/globals.h"
#include "../Utils/LogItem.h"
#include <ctime>
#include <cstdint>
#include <iostream>

class SymbolManager {

    // const uint64_t NANOS_PER_DAY = 24ULL * 60 * 60 *1000000000ULL;

    ShmemManager* mShmemManager;

    LogItem newLog;
    // price data
    Price latest_print_price = 0;
    Price nyse_open_price = 0;

    // quote data
    Price bid_price = 0;
    Price ask_price = 0;

    // imbalance data
    Price clear_price = 0;
    Price imb_exchange = 0;
    // Price far_price = 0;

    //vwap
    // int64_t currentVWAP = 0;
    // int64_t total_exposure = 0;
    // int64_t total_shares = 0;

    // Timestamp data
    Timestamp opening_auction = 0;

    Timestamp current_time = 0;

    // share data
    Shares latest_print_quant = 0;

    // quote data
    // Shares bid_quant = 0;
    // Shares ask_quant = 0;

    // imbalance data
    Shares imb_shares = 0;
    Shares paired_shares = 0;

    SymbolId sym_id = 0;

    SymbolId thread_id = 0;

    side sig_imb_direction = side::BUY;

    bool is_NYSE_open = false;

    bool is_shortable = true;

    bool sig_imbalance = false;

public:

    Shares m_sym_position = 0;
    
    void gotPrint(const MDupdate& _md){
        latest_print_price = _md.m_bid_price;
        latest_print_quant = _md.m_bid_quant;
        if(!is_NYSE_open && _md.m_ask_quant == 10){
            is_NYSE_open = true;
        }
        current_time = _md.m_timestamp;
    }

    void gotNYSEOpen(const MDupdate& _md){
        latest_print_price = _md.m_bid_price;
        nyse_open_price = _md.m_bid_price;
        latest_print_quant = _md.m_bid_quant;
        current_time = _md.m_timestamp;
        is_NYSE_open = true;
    }

    void gotNASDOpen(const MDupdate& _md){
        latest_print_price = _md.m_bid_price;
        latest_print_quant = _md.m_bid_quant;
        current_time = _md.m_timestamp;
    }

    void gotBid(){;}

    void gotAsk(){;}

    void gotQuote(const MDupdate& _md){
        bid_price = _md.m_bid_price;
        // bid_quant = _bid_shares;
        ask_price = _md.m_ask_price;
        // ask_quant = _ask_shares;
        current_time = _md.m_timestamp;
    }

    void gotImbalance(const MDupdate& _md){
        clear_price = _md.m_bid_price;
        imb_shares = _md.m_bid_quant;
        paired_shares = _md.m_ask_quant;
        imb_exchange = _md.m_ask_price;
        current_time = _md.m_timestamp;
    }

    void gotSigImbalance(const MDupdate& _md){
        clear_price = _md.m_bid_price;
        imb_shares = _md.m_bid_quant;
        paired_shares = _md.m_ask_quant;
        imb_exchange = _md.m_ask_price;
        current_time = _md.m_timestamp;

        sig_imbalance = true;

        if(imb_shares > 0)
            sig_imb_direction = side::BUY;
        else
            sig_imb_direction = side::SELL;

        newLog.clear();
        newLog.m_type = log_type::IMBALANCE;
        newLog.m_price = getImbPrice();
        newLog.m_price2 = getMidPoint();
        newLog.m_shares = getPairedShares();
        newLog.m_shares2 = getImbalanceQuant();
        newLog.m_symbolId = getSymbolID();
        newLog.m_delay = getImbExchange();
        newLog.m_current_time = getCurrentTime();

        mShmemManager->pushLog(newLog, thread_id);
    }

    Price getLatestPrintPrice(){
        return latest_print_price;
    }

    Price getBidPrice(){
        return bid_price;
    }

    Price getAskPrice(){
        return ask_price;
    }

    Price getMidPoint(){
        return (bid_price + ask_price) / 2;
    }

    // Price getSpread(){
    //     Price spread = ask_price - bid_price;
    //     if(spread < 0)
    //         return 0;
    //     return spread;
    // }

    Price getImbPrice(){
        return clear_price;
    }

    Price getNYSEOpenPrice(){
        return nyse_open_price;
    }

    Price getImbExchange(){
        return imb_exchange;
    }

    // Price getNearPrice(){
    //     return near_price;
    // }

    // Price getVWAP(){
    //     return currentVWAP;
    // }

    Shares getLatestPrintQuant(){
        return latest_print_quant;
    }

    // Shares getBidQuant(){
    //     return bid_quant;
    // }

    // Shares getAskQuant(){
    //     return ask_quant;
    // }

    Shares getPairedShares(){
        return paired_shares;
    }

    Shares getImbalanceQuant(){
        return imb_shares;
    }

    Timestamp getCurrentTime(){
        return current_time;
    }

    void setCurrentTime(const Timestamp& _timestamp){
        current_time = _timestamp;
    }

    Timestamp getOpeningAuctionTime(){
        return opening_auction;
    }

    bool gotQuotes(){
        if(bid_price == 0) return false;
        if(ask_price == 0) return false;

        return true;
    }

    void OnInit(const SymbolId& _sym_id, const Timestamp& _market_open, const bool& _is_shortable, ShmemManager* _schmemMan){
        sym_id = _sym_id;
        thread_id = _sym_id % TRADE_WTHREADS;
        mShmemManager = _schmemMan;
        opening_auction = _market_open;
        is_shortable = _is_shortable;
    }

    SymbolId getSymbolID(){
        return sym_id;
    }

    SymbolId getThreadID(){
        return thread_id;
    }

    bool isShortable(){
        return is_shortable;
    }

    bool GetIsNYSEOpen(){
        return is_NYSE_open;
    }

    bool SigImbRecieved(){
        return sig_imbalance;
    }

    side GetSigImbDir(){
        return sig_imb_direction;
    }

    void processResp(const Response& currentResp){
        if(currentResp.m_resp_quant == 1)
            is_shortable = true;
        else if(currentResp.m_resp_quant == 1)
            is_shortable = false;
        else
            is_shortable = false;
    }

    // void calculateVWAP(const Price& _print_price, const Shares& _print_shares){
    //     total_exposure += _print_price * int64_t(_print_shares);
    //     total_shares += int64_t(_print_shares);
    //     currentVWAP = total_exposure / total_shares;
    // }
};