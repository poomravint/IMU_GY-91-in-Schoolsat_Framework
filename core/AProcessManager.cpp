/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗ 
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝
                                                                
 File: AProcessManager.cpp  
 Author: Aroonsak Bootchai [AB]
===============================================================
The SchoolSAT systems' main file executes and manages all
processes while controlling the environment. eg. MQueue,SHM,PID
 
 Developer: Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-11
 Version: 100
===============================================================
*/
#include <iostream>
#include <unistd.h>
#include "../include/CMqueue.hpp"
#include "mqueue.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <cstdlib>


int main() {
    std::cout << "████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗ "<< std::endl;
    std::cout << "╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗"<< std::endl;
    std::cout << "   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║"<< std::endl;
    std::cout << "   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║"<< std::endl;
    std::cout << "   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║"<< std::endl;    
    std::cout << "   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝"<< std::endl << std::flush;
    
    std::cout << "Process Manager" << std::endl;
    std::cout << "RESET ENVIRONMENT.." << std::endl <<std::flush;
    std::cout << "DONE" <<std::endl << std::flush;

    std::cout << "initialising environment..." << std::endl << std::flush;

    // @formatter:off
    CMqueue *mq_command = new CMqueue("/command");
    CMqueue *mq_log     = new CMqueue("/log");
    CMqueue *mq_3021    = new CMqueue("/302.1");
    CMqueue *mq_3022    = new CMqueue("/302.2");
    CMqueue *mq_304     = new CMqueue("/304");
    CMqueue *mq_305     = new CMqueue("/305");
    CMqueue *mq_306     = new CMqueue("/306");

    mq_command  ->  OpenRDWR();
    mq_log      ->  OpenRDWR();
    mq_3021     ->  OpenRDWR();
    mq_3022     ->  OpenRDWR();
    mq_304      ->  OpenRDWR();
    mq_305      ->  OpenRDWR();
    mq_306      ->  OpenRDWR();

    mq_command  ->  Close();
    mq_log      ->  Close();
    mq_3021     ->  Close();
    mq_3022     ->  Close();
    mq_304      ->  Close();
    mq_305      ->  Close();
    mq_306      ->  Close();

    std::cout << "mqueue initialised" << std::endl << std::flush;

    // Open the shared memory object
    int shm_3021    = shm_open("/302.1",O_RDWR, 0666);
    int shm_3022    = shm_open("/302.2",O_RDWR, 0666);
    int shm_303     = shm_open("/303",  O_RDWR, 0666);
    int shm_304     = shm_open("/304",  O_RDWR, 0666);
    int shm_305     = shm_open("/305",  O_RDWR, 0666);
    int shm_306     = shm_open("/306",  O_RDWR, 0666);

    close(shm_3021);
    close(shm_3022);
    close(shm_303);
    close(shm_304);
    close(shm_305);
    close(shm_306);

    std::cout << "shared memory initialised" << std::endl << std::flush;
    // @formatter:on

    // for (int i = 0; i < 8; ++i) {
    //     pid_t pid = fork();
        
    //     if (pid < 0) {
    //         std::cerr << "Failed to fork a child process." << std::endl;
    //         return 1;
    //     } else if (pid == 0) {
    //         // Child process
    //         std::cout << "Child process " << i+1 << ". PID: " << getpid() << std::endl;
            
    //         std::string executable;
    //         // Set the executable path based on the child process number (i)
    //         switch (i) {
    //             case 0:
    //                 executable = "child1_executable";
    //                 break;
    //             case 1:
    //                 executable = "child2_executable";
    //                 break;
    //             // Add cases for the remaining child processes...
    //             default:
    //                 executable = "default_executable";
    //                 break;
    //         }
            
    //         execlp(executable.c_str(), executable.c_str(), nullptr);
    //         // If execlp() returns, it means there was an error
    //         std::cerr << "Failed to execute " << executable << std::endl;
    //         return 1;
    //     }
    // }
    
    // // Parent process
    // for (int i = 0; i < 8; ++i) {
    //     // wait(nullptr);  // Wait for each child process to finish
    
    // }
    
    return 0;
}
