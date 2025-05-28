#pragma once

class LogManager {
private:
    static LogManager uniqueInstance;
    LogManager(){;}

public:
    static LogManager getInstance();
};