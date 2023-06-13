#pragma once

/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝

 File: CDatapool.cpp
 Author: Aroonsak Bootchai [AB]
===============================================================
Data pool is the core concept for distributed process systems for spacecraft.
CDataPool will simplified using of POSIX Shared Memory

 Developer: Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-12
 Version: 010
============================================================
*/

// data pool using shared memory mechanism
#include <unordered_map>
#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


class Datapool {
private:
  /* data */
  int shm_fd = 0;
  char* SHM_NAME;
  size_t SHM_SIZE = sizeof(std::unordered_map<int,int32_t>);
  int Open(std::string name){
    // Create or open the POSIX shared memory object
    SHM_NAME = name.data();
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to open shared memory" << std::endl;
        return 1;
    }

    // Resize the shared memory object to accommodate the unordered_map
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        std::cerr << "Failed to resize shared memory" << std::endl;
        return 1;
    }

    // Map the shared memory object into the process's address space
    shm_ptr = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        std::cerr << "Failed to map shared memory" << std::endl;
        return 1;
    }

    // Create an unordered_map in the shared memory region
    tm = new (shm_ptr) std::unordered_map<int,int32_t>();
    return 0;
  };

  int Close(){
    // Unmap the shared memory object
    if (munmap(shm_ptr, SHM_SIZE) == -1) {
        std::cerr << "Failed to unmap shared memory" << std::endl;
        return 1;
    }

    // Close and unlink the shared memory object
    if (close(shm_fd) == -1) {
        std::cerr << "Failed to close shared memory" << std::endl;
        return 1;
    }
    if (shm_unlink(SHM_NAME) == -1) {
        std::cerr << "Failed to unlink shared memory" << std::endl;
        return 1;
    }
  };
public: 
  Datapool(){

  };
  Datapool(const std::string shm_name){
    Open(shm_name);
  };
  ~Datapool(){
    Close();
  };
  void *shm_ptr;
  std::unordered_map<int, int32_t>* tm;
};

