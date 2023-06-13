#pragma once

/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝

 File: CCommand.cpp
 Author: Aroonsak Bootchai [AB]
===============================================================
CCommand is a library of utility functions designed to facilitate
the parsing and structuring of TTC (Telemetry and Telecommand) data. 
This code provides a set of tools and functions that can be used to
process TTC messages, extract relevant information, and organize it
in a structured manner for further analysis or action.

 Developer: Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-12
 Version: 010
===============================================================
*/

#include <string>
#include <iostream>
#include <functional>

#include "CMqueue.hpp"
#include "CDataPool.hpp"
#include "CLog.hpp"
#include "ITTC.hpp"
#include "CLog.hpp"

struct Services {
    CMqueue* MQ_Return;
    Logger* Log;
    Datapool* Pool;
};

// Define a function type with two arguments
typedef std::function<void(TTCInternal*, Services*)> FunctionType;
typedef std::unordered_map <int,FunctionType> TTCType;



struct CommandThreadParams {
    CMqueue* mq_return;
    CMqueue* mq_request;
    CMqueue* mq_logservice;
    TTCType* tc_actions;
    TTCType* tm_actions;
    Datapool* pool;
    Logger* logger;
};


// Function executed by the CommandThread
void* commandThreadFunc(void* arg) {
    std::cout << "posix started" << std::endl << std::flush;
    CommandThreadParams* args = static_cast<CommandThreadParams*>(arg);
    TTCInternal *ttc {0};
    unsigned int priority;
    Services *services;
    services->MQ_Return = args->mq_return;
    services->Log = args->logger;
    services->Pool = args->pool;

    while (true) {
        ssize_t bytesRead = mq_receive(args->mq_request->mq_dest, (char*) &ttc, sizeof(ttc), &priority);
        if (bytesRead == -1) {
            std::cerr << "Failed to receive message from the queue: " << strerror(errno) << std::endl;
            break;
        }
        switch(ttc->ctrlType){
            case CtrlType::TC_req:
            if(args -> tc_actions -> contains((int)ttc->commandID)){
                (*args -> tc_actions)[ttc->commandID](ttc, services);
            }
            break;
            case CtrlType::TM_req:
            if(args -> tm_actions -> contains((int)ttc->commandID)){
                (*args -> tm_actions)[ttc->commandID](ttc, services);
            }
            break;
        }
    }
    pthread_exit(nullptr);
};
