#include <iostream>
#include "TradingEngine.h"

TradingEngine       gTradingEngine;

int main() {
    
    gTradingEngine.startUp();

    gTradingEngine.run();

    gTradingEngine.shutDown();
    
    return 0;
}