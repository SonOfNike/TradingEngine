#pragma once

#include "../Utils/enums_typedef.h"
#include "../Utils/globals.h"
#include <ctime>
#include <cstdint>
#include <iostream>

class SymbolManager {

    const uint64_t NANOS_PER_DAY = 24ULL * 60 * 60 *1000000000ULL;
    // price data
    Price latest_print_price = 0;

    // quote data
    Price bid_price = 0;
    Price ask_price = 0;

    // imbalance data
    Price near_price = 0;
    Price far_price = 0;

    // first 15 minute
    Price first_15_high = 0;
    Price first_15_low = 0;

    //vwap
    Price currentVWAP = 0;
    Price total_exposure = 0;

    // Timestamp data
    Timestamp opening_auction = 0;

    Timestamp current_time = 0;

    // share data
    Shares latest_print_quant = 0;

    // quote data
    Shares bid_quant = 0;
    Shares ask_quant = 0;

    //vwap
    Shares total_shares = 0;

    // imbalance data
    Shares imbalance_quant = 0;
    Shares paired_shares = 0;

    SymbolId sym_id = 0;

    bool is_shortable = true;
public:
    
    void gotPrint(const Price& _print_price, const Shares& _print_shares, const Timestamp& _current_time){
        latest_print_price = _print_price;
        latest_print_quant = _print_shares;
        current_time = _current_time;

        if(_current_time >= opening_auction){
            if(_current_time <= opening_auction + 15 * MINUTES){
                if(first_15_high == 0) 
                    first_15_high = _print_price;
                else if(first_15_high < _print_price)
                    first_15_high = _print_price;
                if(first_15_low == 0) 
                    first_15_low = _print_price;
                else if(first_15_low > _print_price)
                    first_15_low = _print_price;
            }
            calculateVWAP(_print_price,_print_shares);
        }

        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        uint64_t nanos = uint64_t(ts.tv_sec) * 1000000000ULL + ts.tv_nsec;

        if((nanos % NANOS_PER_DAY) - _current_time > 1500000000)
            std::cout << "Latency=" << (nanos % NANOS_PER_DAY) - _current_time << 
            "|CurrentTime=" << _current_time << "\n";
    }

    void gotBid(){;}

    void gotAsk(){;}

    void gotQuote(const Price& _bid_price, const Shares& _bid_shares, const Price& _ask_price, const Shares& _ask_shares, const Timestamp& _current_time){
        bid_price = _bid_price;
        bid_quant = _bid_shares;
        ask_price = _ask_price;
        ask_quant = _ask_shares;
        current_time = _current_time;

        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        uint64_t nanos = uint64_t(ts.tv_sec) * 1000000000ULL + ts.tv_nsec;

        if((nanos % NANOS_PER_DAY) - _current_time > 1500000000)
            std::cout << "Latency=" << (nanos % NANOS_PER_DAY) - _current_time << 
            "|CurrentTime=" << _current_time << "\n";
    }

    void gotImbalance(){;}

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

    Price getSpread(){
        Price spread = ask_price - bid_price;
        if(spread < 0)
            return 0;
        return spread;
    }

    Price getFarPrice(){
        return far_price;
    }

    Price getNearPrice(){
        return near_price;
    }

    Price get15minHigh(){
        return first_15_high;
    }

    Price get15minLow(){
        return first_15_low;
    }

    Price getVWAP(){
        return currentVWAP;
    }

    Shares getLatestPrintQuant(){
        return latest_print_quant;
    }

    Shares getBidQuant(){
        return bid_quant;
    }

    Shares getAskQuant(){
        return ask_quant;
    }

    Shares getPairedShares(){
        return paired_shares;
    }

    Shares getImbalanceQuant(){
        return imbalance_quant;
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

    void OnInit(const SymbolId& _sym_id, const Timestamp& _market_open, const bool& _is_shortable){
        sym_id = _sym_id;
        opening_auction = _market_open;
        is_shortable = _is_shortable;
    }

    SymbolId getSymbolID(){
        return sym_id;
    }

    bool isShortable(){
        return is_shortable;
    }

    void calculateVWAP(const Price& _print_price, const Shares& _print_shares){
        total_exposure += _print_price * _print_shares;
        total_shares += _print_shares;
        currentVWAP = total_exposure / total_shares;
    }
};