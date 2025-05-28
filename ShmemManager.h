#pragma once

#include "../Utils/MDShmem.h"
#include "../Utils/ReqShmem.h"
#include "../Utils/RespShmem.h"

class ShmemManager {
private:
    MDShmem* md_shmem = nullptr;
    ReqShmem* req_shmem = nullptr;
    RespShmem* resp_shmem = nullptr;

    static ShmemManager* uniqueInstance;
    ShmemManager(){;}

public:
    static ShmemManager* getInstance();
    void startUp();
    void shutDown();
};