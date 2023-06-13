#pragma once

/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝

 File: CExecutor.cpp
 Author: Aroonsak Bootchai [AB]
===============================================================
Main executable which will control about runtime environment of the applications
on Linux system, controlled Arguments, 

 Developer: Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-12
 Version: 010
============================================================
*/

#include <string>
#include <fmt/format.h>
#include <iostream>

//PROMPT> C++20 code, read and parse arguments as follows { -m int , ModuleId -c string ,Configuration file (needed to be .cfg) }
struct Args {
    int moduleId;
    std::string configFile;
    std::string mqueue;
};

Args parseArguments(int argc, char* argv[]) {
    Args args;
    args.moduleId = -1;
    
    for (int i = 1; i < argc; i += 2) {
        std::string option = argv[i];
        
        if (option == "-m" && i + 1 < argc) {
            args.moduleId = std::stoi(argv[i + 1]);
            args.mqueue = fmt::format("/{}",args.moduleId);
        } else if (option == "-c" && i + 1 < argc) {
            args.configFile = argv[i + 1];
        }
    }
    if (args.moduleId == -1 || args.configFile.empty()) {
        std::cout << "Invalid arguments. Usage: program_name -m <ModuleId> -c <Configuration file (.cfg)>\n";
        return args;
    }
    return args;
}