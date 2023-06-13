#include <iostream>
#include <string>
#include <cstdint>

void readArguments(int argc, char* argv[], std::string& configFile, int& pid, uint8_t& module) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                configFile = argv[i + 1];
                ++i;  // Skip the next argument
            } else {
                std::cerr << "Error: Missing filename after --config/-c option." << std::endl;
                // You can choose to exit the program or handle the error differently.
            }
        } else if (arg == "--pid" || arg == "-p") {
            if (i + 1 < argc) {
                pid = std::stoi(argv[i + 1]);
                ++i;  // Skip the next argument
            } else {
                std::cerr << "Error: Missing number after --pid/-p option." << std::endl;
                // You can choose to exit the program or handle the error differently.
            }
        } else if (arg == "--module" || arg == "-m") {
            if (i + 1 < argc) {
                module = static_cast<uint8_t>(std::stoi(argv[i + 1]));
                ++i;  // Skip the next argument
            } else {
                std::cerr << "Error: Missing number after --module/-m option." << std::endl;
                // You can choose to exit the program or handle the error differently.
            }
        }
    }
}

int main(int argc, char* argv[]) {
    std::string configFile;
    int pid = 0;
    uint8_t module = 0;

    readArguments(argc, argv, configFile, pid, module);

    // Now you can use the values read from the command-line arguments
    // For example:
    std::cout << "Config File: " << configFile << std::endl;
    std::cout << "PID: " << pid << std::endl;
    std::cout << "Module: " << static_cast<int>(module) << std::endl;


    return 0;
}