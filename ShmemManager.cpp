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

    next_md_read_index = md_shmem->next_write_index;
    next_md_read_page = md_shmem->next_write_page;

    shm_size = sizeof(ReqShmem);

    // Create or open the shared memory object
    shm_fd = shm_open(REQ_shm_name, O_RDWR, 0666);
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
    shm_fd = shm_open(LOG_shm_name, O_RDWR, 0666);
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
    shm_fd = shm_open(RESP_shm_name, O_RDWR, 0666);
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

    next_resp_read_index = resp_shmem->next_write_index;
    next_resp_read_page = resp_shmem->next_write_page;
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

bool ShmemManager::gotMD(){
    if(md_shmem->next_write_index == next_md_read_index &&
       md_shmem->next_write_page == next_md_read_page)
        return false;
    return true;
}

bool ShmemManager::gotResp(){
    if(resp_shmem->next_write_index == next_resp_read_index &&
       resp_shmem->next_write_page == next_resp_read_page)
        return false;
    return true;
}

void ShmemManager::getMD(MDupdate& newMD){
    newMD = md_shmem->m_queue[next_md_read_index];
    next_md_read_index++;
    if(next_md_read_index >= MD_QUEUE_SIZE){
        next_md_read_index = 0;
        next_md_read_page++;
    }
}

void ShmemManager::getResp(Response& newResp){
    newResp = resp_shmem->m_queue[next_resp_read_index];
    next_resp_read_index++;
    if(next_resp_read_index >= RESP_QUEUE_SIZE){
        next_resp_read_index = 0;
        next_resp_read_page++;
    }
}

void ShmemManager::pushReq(const Request& newReq){
    req_shmem->m_queue[req_shmem->next_write_index] = newReq;
    req_shmem->next_write_index++;
    if(req_shmem->next_write_index >= REQ_QUEUE_SIZE){
        req_shmem->next_write_index = 0;
        req_shmem->next_write_page++;
    }
}

void ShmemManager::pushLog(const LogItem& newLog){
    log_shmem->m_queue[log_shmem->next_write_index] = newLog;
    log_shmem->next_write_index++;
    if(log_shmem->next_write_index >= LOG_QUEUE_SIZE){
        log_shmem->next_write_index = 0;
        log_shmem->next_write_page++;
    }
}

OrderId ShmemManager::getNextOrderID(){
    req_shmem->next_order_id++;
    return req_shmem->next_order_id;
}