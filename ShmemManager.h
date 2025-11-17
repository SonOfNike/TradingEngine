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

class ShmemManager {
private:
    MDShmem* md_shmem = nullptr;
    int32_t next_md_read_index = 0;
    int32_t next_md_read_page = 0;
    ReqShmem* req_shmem = nullptr;
    LogShmem* log_shmem = nullptr;
    RespShmem* resp_shmem = nullptr;
    int32_t next_resp_read_index = 0;
    int32_t next_resp_read_page = 0;
    ErrorShmem* error_shmem = nullptr;
    int32_t next_error_read_index = 0;
    int32_t next_error_read_page = 0;

    static ShmemManager* uniqueInstance;
    ShmemManager(){;}

public:
    static ShmemManager* getInstance();
    void startUp();
    void shutDown();
    bool gotMD();
    bool gotResp();
    bool gotError();
    void getMD(MDupdate& newMD);
    void getResp(Response& newResp);
    void getError(Response& newResp);
    void pushReq(const Request& newReq);
    void pushLog(const LogItem& newLog);
    OrderId getNextOrderID();
};