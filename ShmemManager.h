#pragma once

#include "../Utils/MDShmem.h"
#include "../Utils/ReqShmem.h"
#include "../Utils/RespShmem.h"
#include "../Utils/ErrorShmem.h"
#include "../Utils/LogShmem.h"
#include "../Utils/Response.h"
#include "../Utils/Request.h"
#include "../Utils/MDupdate.h"
#include "../Utils/LogItem.h"
#include <array>

struct alignas(64) ShmemThreadContext{
    int32_t next_md_read_index = 0;
    int32_t next_resp_read_index = 0;
};

class ShmemManager {
private:
    static ShmemManager* uniqueInstance;

    MDShmem* md_shmem = nullptr;
    ReqShmem* req_shmem = nullptr;
    LogShmem* log_shmem = nullptr;
    RespShmem* resp_shmem = nullptr;
    // int32_t next_md_read_index[5] = {0,0,0,0,0};
    // // int32_t next_md_read_page = 0;
    // int32_t next_resp_read_index[5] = {0,0,0,0,0};
    // int32_t next_resp_read_page = 0;

    std::array<ShmemThreadContext, TRADE_WTHREADS> mThreadContexts;

    ShmemManager(){;}

public:
    static ShmemManager* getInstance();
    void startUp();
    void shutDown();
    bool gotMD(const int& index);
    bool gotResp(const int& index);
    void getMD(MDupdate& newMD, const int& index);
    void getResp(Response& newResp, const int& index);
    void pushReq(const Request& newReq, const int& index);
    void pushLog(const LogItem& newLog, const int& index);
    MyOrderId getNextOrderID(const int& index);
};