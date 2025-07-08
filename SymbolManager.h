#pragma once

#include "../Utils/enums_typedef.h"

class SymbolManager {
    SymbolId sym_id = 0;
    // latest print data
    Price latest_print_price = 0;
    Shares latest_print_quant = 0;

    // quote data
    Price bid_price = 0;
    Price ask_price = 0;
    Shares bid_quant = 0;
    Shares ask_quant = 0;

    // imbalance data
    Price near_price = 0;
    Price far_price = 0;
    Shares imbalance_quant = 0;
    Shares paired_shares = 0;

    Timestamp current_time = 0;
public:
    Timestamp opening_auction = 0;
    void gotPrint(Price _print_price, Shares _print_shares, Timestamp _current_time){
        latest_print_price = _print_price;
        latest_print_quant = _print_shares;
        current_time = _current_time;
    }

    void gotBid(){;}

    void gotAsk(){;}

    void gotQuote(Price _bid_price, Shares _bid_shares, Price _ask_price, Shares _ask_shares, Timestamp _current_time){
        bid_price = _bid_price;
        bid_quant = _bid_shares;
        ask_price = _ask_price;
        ask_quant = _ask_shares;
        current_time = _current_time;
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

    Timestamp getOpeningAuctionTime(){
        return opening_auction;
    }

    bool gotQuotes(){
        if(bid_price == 0) return false;
        if(ask_price == 0) return false;

        return true;
    }

    SymbolId getSymbolID(){
        return sym_id;
    }
};