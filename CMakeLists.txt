cmake_minimum_required(VERSION 3.10.0)
project(TradingEngine VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenSSL REQUIRED)
find_package(glog REQUIRED PATHS ../glog/build/)

add_executable(TradingEngine 
    main.cpp
    TradingEngine.cpp
    ShmemManager.cpp
    StrategyManager.cpp
    BaseStrategy.cpp
    ../ReqAlpacaAPI/config.cpp
    ../ReqAlpacaAPI/client.cpp
    ../ReqAlpacaAPI/account.cpp
    ../ReqAlpacaAPI/status.cpp
    ../ReqAlpacaAPI/asset.cpp
    ../ReqAlpacaAPI/order.cpp
    ../Utils/SymbolIDManager.cpp
    ../Utils/simdjson/simdjson.cpp)

target_link_libraries(TradingEngine PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(TradingEngine PRIVATE glog::glog)
target_include_directories(TradingEngine PUBLIC
        ../rapidjson/include
    )

include(CTest)
enable_testing()

set_property(TARGET TradingEngine PROPERTY CXX_STANDARD 17)