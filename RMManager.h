#pragma once

#include "../Utils/enums_typedef.h"

class RMManager {
private:
    static RMManager* uniqueInstance;
    RMManager(){;}

    Price m_global_exposure = 0;
    Price m_global_pnl = 0;

    Price m_global_exposure_limit = 0;
    Price m_global_pnl_limit = 0;
    Price m_sym_pnl_limit = 0;

public:
    static RMManager* getInstance();
    void startUp();
    void shutDown();
    void subtractExposure(Price _exposure);
    void addExposure(Price _exposure);
    void subtractPNL(Price _pnl);
    void addPNL(Price _pnl);
    bool exposureLimitBreached();
    bool pnlLimitBreached();
    Price getSymPnLLimit();
};