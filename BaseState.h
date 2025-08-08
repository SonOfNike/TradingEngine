#pragma once

class BaseStrategy;

class BaseState{
public:
    BaseStrategy* m_strat;

    BaseState(BaseStrategy* _strat) : m_strat(_strat){;}
    
    virtual void run() = 0;
    virtual void gotPrint() = 0;
    virtual void gotQuote() = 0;
    virtual void gotImbalance() = 0;
    virtual void gotTimeout() = 0;
};