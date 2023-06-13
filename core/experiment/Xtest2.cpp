#include <fcntl.h>
#include <sys/mman.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <thread>

int main() {
  while (true) {
    std::cout << "test2" << std::endl << std::flush;
    std::ofstream outfile(
        "test2.txt",
        std::ios::app);  // create an output file stream for example.txt

    time_t current_time = time(nullptr);

    outfile << static_cast<long int>(current_time)
            << std::endl;  // write some text to the file

    outfile.close();  // close the file
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
}