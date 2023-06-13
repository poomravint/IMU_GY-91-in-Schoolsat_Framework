#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <cstdio>
#include <ctime> 
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>

int main() {
  while (true) {
    std::cout << "test1" << std::endl << std::flush;
    std::ofstream outfile("test1.txt",std::ios::app);  // create an output file stream for example.txt

    if (!outfile)  // check if the file was opened successfully
    {
      std::cerr << "Failed to open file\n";
      return 1;
    }

    time_t current_time = time(nullptr); 

    outfile << static_cast<long int>(current_time) << std::endl;  // write some text to the file

    outfile.close();  // close the file
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}