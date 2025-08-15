#include <iostream>
#include "TradingEngine.h"

TradingEngine       gTradingEngine;

int main() {
    
    gTradingEngine.startUp();

    gTradingEngine.run();

    gTradingEngine.shutDown();
    
    return 0;
}

// // main.cpp
// #include "fast_logger.h"
// #include <iostream>

// int main() {
//     auto& L = FastLogger::instance();
//     L.addSink(std::make_unique<FileSink>("fastlog.txt"));
//     L.start();

//     // attach per-thread logger (main thread)
//     ThreadLogger tlog;

//     // simple loop that logs a lot without allocating
//     for (int i = 0; i < 1000000; ++i) {
//         char buf[128];
//         int n = snprintf(buf, sizeof(buf), "loop %d some debug data", i);
//         L.log(tlog.tb, buf);
//     }

//     // simulate worker threads
//     std::thread worker([&]() {
//         ThreadLogger tl; // each thread attach
//         for (int i = 0; i < 500000; ++i) {
//             char b[128];
//             int n = snprintf(b, sizeof(b), "worker iter %d", i);
//             FastLogger::instance().log(tl.tb, b);
//         }
//     });

//     worker.join();

//     // stop logger (flushes)
//     L.stop();

//     std::cout << "Dropped logs: " << L.dropped() << "\n";
//     return 0;
// }