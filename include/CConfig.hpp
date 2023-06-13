#pragma once

/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝

 File: CConfig.cpp
 Author: Aroonsak Bootchai [AB]
===============================================================
Config parser as a Config struct.

 Developer: Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-12
 Version: 010
============================================================
*/

#include <iostream>
#include <string>
#include <fstream>
#include <map>

//PROMPT > write C++20 code that read the file (.cfg) and parse the config and store in pointer
struct Config {
    std::map<std::string, std::string> data;
};

Config* parseConfigFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Failed to open configuration file.\n";
            return nullptr;
        }
        
        Config* config = new Config();
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#')
                continue;
            
            // Find the position of the equal sign
            size_t equalPos = line.find('=');
            if (equalPos != std::string::npos) {
                // Split the line into key and value
                std::string key = line.substr(0, equalPos);
                std::string value = line.substr(equalPos + 1);
                
                // Store key-value pair in the configuration data
                config->data[key] = value;
            }
        }
        file.close();
        std::cout << "loaded config "<< sizeof(config) << std::endl;
        return config;
   
}