#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main() {
    pid_t pid1, pid2;
    int status1, status2;
    std::ifstream file("startup.txt");
    std::string line;
    std::vector<pid_t> pid_vec = {};
    std::vector<std::string> process = {};
    

    if (file.is_open()) {
        while (std::getline(file, line)) {
            process.push_back(line);
        }
        file.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }

    std::cout << "Processes to be run : " << process.size() << std::endl;
    
    for (int i = 0; i < process.size(); i++) {
        pid_t pid = fork();
        if(pid == 0){
            pid_vec.push_back(pid);
            execlp(process[i].c_str(),"argument1",NULL);
            exit(0);
        }
    }

    for (int i = 0; i < pid_vec.size(); i++) {
        waitpid(pid_vec[i],nullptr,0);
    }

    while(true){}
    return 0;
}
