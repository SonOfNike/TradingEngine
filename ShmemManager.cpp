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
    req_shmem = (ReqShmem*)mmap(0, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (md_shmem == MAP_FAILED) {
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
}

void ShmemManager::shutDown(){
    if (munmap(md_shmem, sizeof(MDShmem)) == -1) {
        perror("munmap");
    }

    if (munmap(req_shmem, sizeof(ReqShmem)) == -1) {
        perror("munmap");
    }

    if (munmap(resp_shmem, sizeof(RespShmem)) == -1) {
        perror("munmap");
    }
}