#include <iostream>
#include "TradingEngine.h"
#include "../Utils/pin_thread_functions.h"

TradingEngine       gTradingEngine;

int main() {

    std::vector<std::thread> threads;
    
    gTradingEngine.startUp();

    for(int i = 0; i < TRADE_WTHREADS - 1; i++){
        threads.emplace_back(&TradingEngine::run, &gTradingEngine, i);
        pin_thread_to_core(threads[i], i);
    }

    pin_current_thread(TRADE_WTHREADS - 1);

    // std::thread t1(&TradingEngine::run, &gTradingEngine, 1);
    // std::thread t2(&TradingEngine::run, &gTradingEngine, 2);
    // std::thread t3(&TradingEngine::run, &gTradingEngine, 3);
    // std::thread t4(&TradingEngine::run, &gTradingEngine, 4);
    // std::thread t5(&TradingEngine::run, &gTradingEngine, 5);

    gTradingEngine.run(TRADE_WTHREADS - 1);

    gTradingEngine.shutDown();
    
    return 0;
}