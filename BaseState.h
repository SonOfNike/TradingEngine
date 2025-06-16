#pragma once

class BaseState{
public:
    virtual void run() = 0;
    virtual void gotPrint() = 0;
    virtual void gotQuote() = 0;
    virtual void gotImbalance() = 0;
};