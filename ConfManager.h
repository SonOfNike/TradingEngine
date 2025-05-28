#pragma once

class ConfManager {
private:
    static ConfManager uniqueInstance;
    ConfManager(){;}

public:
    static ConfManager getInstance();
};