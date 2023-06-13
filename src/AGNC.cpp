/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗ 
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝
                                                                
 File: AGNC.cpp  
 Author: [name]
===============================================================
 [description here]
 
 Developer: [name], Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-05
 Version: 000
===============================================================
*/

/**
 * [x] application shall execute with minimum required arguments
 * [x] application shall able to load config file as mentioned from aguments
 * [x] the config file shall be shown registered TC and registered TM (which validated from FSW)
 * [x] the TC and TM in unordered_map (which will able to use find key)
 * [ ] every switch, boolean, shared data, shared value can be in form of atomic & shared pointers
 * - Aroonsak Bootchai
*/

/** At main thread,
 * 1 ) [X] read and check arguments is correct from defined
 * 2 ) [X] read file from argument, the config shall be parsed from the file then close file
 * 3 ) [X] read config, 
 * [ ] if key have __TC__, parse the line by TC::parse then push TC to unordered_map TCTable
 * [ ] if key have __TM__, parse the line by TM::parse then push TM to unordered_map TMTable
 * [ ] else push the config to unordered_map Config 
 * 4 ) [x] from config, parse the line by Config::parse then push Config to unordered_map ConfigTable
 * 5 ) [x] TMFunction (unordered_map)
 * 6 ) [x] TCFunction (unordered_map)
 * 7 ) [x] detach a thread, Command Thread with (reference)TMTable, (reference)TCTable, (reference)TMFUnction, (reference)TCFunction and Config as arguments to thread
 * 8 ) [x] while loop to prevent the main thread stopped
 * - Aroonsak Bootchai
*/

/** 1 [ ] detached thread (Pthread) named Command thread,
 *  reading message from message queue (using Config["mqueue"] as name ) and parse that to MsgPacket struct
 * (message packet is a struct that  ...), after parsed,
 * [ ] if MsgPacket.ctrlType == 0 then find MsgPacket.commandID in TMFunction, exec TCFunc(MsgPacket)
 * [ ] if MsgPacket.ctrlType == 2 then find MsgPacket.commandID in TCFunction, exec TMFunc(MsgPacket)
 * - Aroonsak Bootchai
**/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <fmt/format.h>
#include <atomic>
#include <mqueue.h>
#include <functional>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>

#include "../include/CExecutor.hpp"
#include "../include/CConfig.hpp"
#include "../include/CLog.hpp"
#include "../include/CDataPool.hpp"
#include "../include/CMqueue.hpp"
#include "../include/ITTC.hpp"
#include "../include/CCommand.hpp"


std::atomic<uint8_t> thread_status = 0;

int GNC_TC_1 (const TTCInternal* ttc,const Services* services) {
    //working some thing here
    printf("working TC_1");
    thread_status = 1;
    std::thread t1([=](){
        while(thread_status == 1){
            // write a debug log
            //store data to datapool as [tmID] = [value]
            (*services->Pool->tm)[0] = 100;
            (*services->Pool->tm)[1] = 100;
            (*services->Pool->tm)[2] = 100;
            (*services->Pool->tm)[3] = 100;
            //sleep thread to prevent over perform
            TTCInternal ttc = ttc;
            ttc.ctrlType = CtrlType::TC_ret;
            services->MQ_Return->Send(ttc);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        // printf("thread closed");
        std::cout <<"thread TC_1 closed" << std::endl << std::flush;
        return 0;
    });
    t1.detach();
    return 0;
};



int main(int argc, char* argv[]) {
    Args args = parseArguments(argc, argv);
    Config* config = parseConfigFile(args.configFile);
    if (!config) {
        return 1;
    }

    Logger *logger = new Logger();
    logger -> setLogLevel(LogLevel::DEBUG);
    logger -> setModuleID(3021);
    logger -> log(LogLevel::DEBUG, "This text will be logged");

    Datapool *pool = new Datapool("/302.1");

    CMqueue *mq_request = new CMqueue("/302.1");
    CMqueue *mq_return = new CMqueue("/308");
    CMqueue *mq_log = new CMqueue("/log");

    if (mq_return->OpenWrite() == -1) {
        logger -> log(LogLevel::ERROR, "Failed to open message queue");
        return EXIT_FAILURE;
    }else{
        logger -> log(LogLevel::INFO, "connected MQ_Return");
    }

    if (mq_request->OpenRead() == -1) {
        logger -> log(LogLevel::ERROR, "Failed to open message queue");
        return EXIT_FAILURE;
    }else{
        logger -> log(LogLevel::INFO, "connected MQ_Request");
    }

    if (mq_log->OpenWrite() == -1) {
        logger -> log(LogLevel::ERROR, "Failed to open message queue");
        return EXIT_FAILURE;
    }else{
        logger -> log(LogLevel::INFO, "connected MQ_Logger");
    }
    // ======================= Start subsystem code ========================================

    std::unordered_map <int,std::function<void(TTCInternal*,Services*)>> tc_actions;
    // tc_actions[<<commandID>>] = function()
    tc_actions[1] = GNC_TC_1;

    // ===============================================================
    CommandThreadParams *params = new CommandThreadParams;
    params->mq_return = mq_return;
    params->mq_request = mq_request;
    params->mq_logservice = mq_log;
    params->tc_actions = &tc_actions;
    params->pool = pool;
    params->logger = logger;

    // Start the CommandThread as a detached thread
    pthread_t commandThread;
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
    pthread_create(&commandThread, &threadAttr, commandThreadFunc, params); 

    
    int input;
    while (true) {
        // Do other tasks if needed
        std::cin >> input;
    }

    pthread_attr_destroy(&threadAttr);
    return 0;
}
