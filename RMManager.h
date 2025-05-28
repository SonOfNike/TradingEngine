#pragma once

class RMManager {
private:
    static RMManager uniqueInstance;
    RMManager(){;}

public:
    static RMManager getInstance();
};