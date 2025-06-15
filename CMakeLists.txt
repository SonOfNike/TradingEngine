cmake_minimum_required(VERSION 3.10.0)
project(TradingEngine VERSION 0.1.0 LANGUAGES C CXX)

add_executable(TradingEngine 
    main.cpp
    TradingEngine.cpp
    ShmemManager.cpp
    ../Utils/SymbolIDManager.cpp
    ../Utils/simdjson/simdjson.cpp)

include(CTest)
enable_testing()

set_property(TARGET TradingEngine PROPERTY CXX_STANDARD 17)