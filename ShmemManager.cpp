#include "ShmemManager.h"
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

ShmemManager* ShmemManager::uniqueInstance = nullptr;

ShmemManager* ShmemManager::getInstance(){
    if(uniqueInstance == nullptr){
        uniqueInstance = new ShmemManager();
    }
    return uniqueInstance;
}

void ShmemManager::startUp(){
    int shm_fd;
    size_t shm_size = sizeof(MDShmem);

    // Create or open the shared memory object
    shm_fd = shm_open(MD_shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
    }

    // Configure the size of the shared memory object
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
    }

    // Map the shared memory object into the process's address space
    md_shmem = (MDShmem*)mmap(0, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (md_shmem == MAP_FAILED) {
        perror("mmap");
    }
    close(shm_fd);

    for(int i = 0; i < TRADE_WTHREADS; i++){
        mThreadContexts[i].next_md_read_index = md_shmem->slot[i].next_write_index.load(std::memory_order_acquire);
    }

    // next_md_read_page = md_shmem->next_write_page;

    shm_size = sizeof(ReqShmem);

    // Create or open the shared memory object
    //Alpaca
    // shm_fd = shm_open(REQ_shm_name, O_RDWR, 0666);

    //IB
    shm_fd = shm_open(IBREQ_shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
    }

    // Configure the size of the shared memory object
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
    }

    // Map the shared memory object into the process's address space
    req_shmem = (ReqShmem*)mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (req_shmem == MAP_FAILED) {
        perror("mmap");
    }
    close(shm_fd);

    shm_size = sizeof(LogShmem);

    // Create or open the shared memory object
    //Alpaca
    // shm_fd = shm_open(LOG_shm_name, O_RDWR, 0666);

    //IB
    shm_fd = shm_open(IBLOG_shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
    }

    // Configure the size of the shared memory object
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
    }

    // Map the shared memory object into the process's address space
    log_shmem = (LogShmem*)mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (log_shmem == MAP_FAILED) {
        perror("mmap");
    }
    close(shm_fd);

    shm_size = sizeof(RespShmem);
    // Create or open the shared memory object
    //Alpaca
    // shm_fd = shm_open(RESP_shm_name, O_RDWR, 0666);

    //IB
    shm_fd = shm_open(IBRESP_shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
    }

    // Configure the size of the shared memory object
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate");
    }

    // Map the shared memory object into the process's address space
    resp_shmem = (RespShmem*)mmap(0, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (md_shmem == MAP_FAILED) {
        perror("mmap");
    }
    close(shm_fd);

    for(int i = 0; i < TRADE_WTHREADS; i++){
        mThreadContexts[i].next_resp_read_index= resp_shmem->slot[i].next_write_index.load(std::memory_order_acquire);
    }
    
    // next_resp_read_page = resp_shmem->next_write_page;
}

void ShmemManager::shutDown(){
    if (munmap(md_shmem, sizeof(MDShmem)) == -1) {
        perror("munmap");
    }

    if (munmap(req_shmem, sizeof(ReqShmem)) == -1) {
        perror("munmap");
    }

    if (munmap(log_shmem, sizeof(LogShmem)) == -1) {
        perror("munmap");
    }

    if (munmap(resp_shmem, sizeof(RespShmem)) == -1) {
        perror("munmap");
    }
}

bool ShmemManager::gotMD(const int& index){
    if(md_shmem->slot[index].next_write_index.load(std::memory_order_acquire) == mThreadContexts[index].next_md_read_index)
        return false;
    return true;
}

bool ShmemManager::gotResp(const int& index){
    if(resp_shmem->slot[index].next_write_index.load(std::memory_order_acquire) == mThreadContexts[index].next_resp_read_index)
        return false;
    return true;
}

void ShmemManager::getMD(MDupdate& newMD, const int& index){
    newMD = md_shmem->slot[index].m_queue[mThreadContexts[index].next_md_read_index];
    mThreadContexts[index].next_md_read_index++;
    if(mThreadContexts[index].next_md_read_index >= MD_QUEUE_SIZE){
        mThreadContexts[index].next_md_read_index = 0;
        // next_md_read_page++;
    }
}

void ShmemManager::getResp(Response& newResp, const int& index){
    newResp = resp_shmem->slot[index].m_queue[mThreadContexts[index].next_resp_read_index];
    mThreadContexts[index].next_resp_read_index++;
    if(mThreadContexts[index].next_resp_read_index >= RESP_QUEUE_SIZE){
        mThreadContexts[index].next_resp_read_index = 0;
        // next_resp_read_page++;
    }
}

void ShmemManager::pushReq(const Request& newReq, const int& index){
    req_shmem->slot[index].m_queue[req_shmem->slot[index].next_write_index] = newReq;
    auto cur = req_shmem->slot[index].next_write_index.load(std::memory_order_relaxed);
    req_shmem->slot[index].next_write_index.store(cur + 1, std::memory_order_release);
    // req_shmem->slot[index].next_write_index.fetch_add(1, std::memory_order_release);
    if(req_shmem->slot[index].next_write_index >= REQ_QUEUE_SIZE){
        req_shmem->slot[index].next_write_index = 0;
        req_shmem->slot[index].next_write_page++;
    }
}

void ShmemManager::pushLog(const LogItem& newLog, const int& index){
    log_shmem->slot[index].m_queue[log_shmem->slot[index].next_write_index] = newLog;
    auto cur = log_shmem->slot[index].next_write_index.load(std::memory_order_relaxed);
    log_shmem->slot[index].next_write_index.store(cur + 1, std::memory_order_release);
    // log_shmem->slot[index].next_write_index.fetch_add(1, std::memory_order_release);
    if(log_shmem->slot[index].next_write_index >= LOG_QUEUE_SIZE){
        log_shmem->slot[index].next_write_index = 0;
        log_shmem->slot[index].next_write_page++;
    }
}

MyOrderId ShmemManager::getNextOrderID(const int& index){
    req_shmem->slot[index].next_order_id++;
    return req_shmem->slot[index].next_order_id;

    // MyOrderId return_val = req_shmem->next_order_id.fetch_add(1, std::memory_order_relaxed) + 1;
    // return return_val;
}