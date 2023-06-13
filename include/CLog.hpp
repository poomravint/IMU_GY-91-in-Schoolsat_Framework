#pragma once

/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝

 File: CLog.cpp
 Author: Aroonsak Bootchai [AB]
===============================================================
CLog is a library of utility functions designed to facilitate
Log controls in the spacecraft

 Developer: Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-12
 Version: 010
===============================================================
*/

#include <iostream>
#include <mqueue.h>
#include <string>
#include <cerrno>
#include <cstring>
#include <chrono>
#include <fmt/format.h>

enum class LogLevel:uint8_t {
  DEBUG,
  INFO,
  WARNING,
  ERROR
};

struct LogMessage {
  uint32_t timestamp;
  LogLevel level;
  int moduleId;
  std::string message;
};

class Logger {
public:
    bool mq_service{false};
    void setLogLevel(LogLevel level) {
        logLevel = level;
    }

    void setModuleID(int num){
        module_id = num;
    }

    void setService(const char *name){
       mq_service = true;
        // Open the message queue
       mq_log = mq_open("/log", O_WRONLY | O_CREAT, 0666, nullptr);
        if (mq_log == -1) {
            std::cerr << "Failed to open message queue: " << strerror(errno) << std::endl << std::flush;
        }
    }

    void log(const std::string& message){
        log(LogLevel::DEBUG, message);
    }

    void log(LogLevel level, const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto unixTimestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

        LogMessage msg = {.timestamp = static_cast<uint32_t>(unixTimestamp),.level = level,.moduleId = module_id,.message = message };
        if(mq_service){
            if (mq_send(mq_log, message.c_str(), message.length() + 1, 1) == -1) {
                std::cerr << "Failed to send message to the queue." << std::endl << std::flush;
            }
        }
        if (level >= logLevel) {
            if(level == LogLevel::ERROR){
                std::cerr << "ERROR: " << message << std::endl << std::flush;
            }else{
                std::cout << fmt::format("{:4} {:5} {}", module_id, getLogLevelString(level), message).c_str() << std::endl << std::flush;
            }
        }
    }

private:
    mqd_t mq_log;
    int module_id;
    LogLevel logLevel = LogLevel::INFO;

    std::string getLogLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARNING:
                return "WARN";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::DEBUG:
            default:
                return "DEBUG";
        }
    }
};