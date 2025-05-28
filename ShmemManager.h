#pragma once

#include "../Utils/MDShmem.h"
#include "../Utils/ReqShmem.h"
#include "../Utils/RespShmem.h"
#include "../Utils/Response.h"
#include "../Utils/Request.h"
#include "../Utils/MDupdate.h"

class ShmemManager {
private:
    MDShmem* md_shmem = nullptr;
    int32_t next_md_read_index = 0;
    int32_t next_md_read_page = 0;
    ReqShmem* req_shmem = nullptr;
    RespShmem* resp_shmem = nullptr;
    int32_t next_resp_read_index = 0;
    int32_t next_resp_read_page = 0;

    static ShmemManager* uniqueInstance;
    ShmemManager(){;}

public:
    static ShmemManager* getInstance();
    void startUp();
    void shutDown();
    bool gotMD();
    bool gotResp();
    void getMD(MDupdate& newMD);
    void getResp(Response& newResp);
    void pushReq(const Request& newReq);
};