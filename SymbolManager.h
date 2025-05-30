#pragma once

#include "../Utils/enums_typedef.h"

class SymbolManager {
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

    void gotPrint(){

    }

    void gotBid(){

    }

    void gotAsk(){

    }

    void gotImbalance(){
        
    }

public:
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
}