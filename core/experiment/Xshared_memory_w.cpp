#include <iostream>
#include <cstdlib>
#include <ctime>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// Structure to hold the shared memory data
struct SharedData {
    int value;
};

// Function to generate a random number between min and max
int generateRandomNumber(int min, int max) {
    return min + (rand() % (max - min + 1));
}

// Function to be executed by the first thread
void* writerThreadFunc(void* arg) {
    SharedData* sharedData = reinterpret_cast<SharedData*>(arg);

    while (true) {
        int value = generateRandomNumber(100, 999);

        // Write value to the shared memory
        sharedData->value = value;

        std::cout << "Writer thread: Wrote value " << value << std::endl;

        sleep(2); // Sleep for 2 seconds
    }

    return nullptr;
}

// Function to be executed by the second thread
void* readerThreadFunc(void* arg) {
    SharedData* sharedData = reinterpret_cast<SharedData*>(arg);

    while (true) {
        // Read value from the shared memory
        int value = sharedData->value;

        std::cout << "Reader thread: Read value " << value << std::endl;

        sleep(1); // Sleep for 1 second
    }

    return nullptr;
}

int main() {
    // Create shared memory
    int shmfd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shmfd, sizeof(SharedData));
    SharedData* sharedData = reinterpret_cast<SharedData*>(mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0));

    // Initialize shared data
    sharedData->value = 0;

    // Create threads
    pthread_t writerThread, readerThread;
    pthread_create(&writerThread, nullptr, writerThreadFunc, sharedData);
    pthread_create(&readerThread, nullptr, readerThreadFunc, sharedData);

    // Wait for threads to finish
    pthread_join(writerThread, nullptr);
    pthread_join(readerThread, nullptr);

    // Cleanup shared memory
    munmap(sharedData, sizeof(SharedData));
    shm_unlink("/my_shared_memory");

    return 0;
}