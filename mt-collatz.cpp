#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <vector>
#include <string> 
#include <iomanip>

// Shared global variablles
unsigned int COUNTER = 1;
unsigned int n = 1;
unsigned short int t = 1; // Num of threads
bool lock = true; // False enables race conditions. 
unsigned int histogram[1001] = {0}; // Initializes all values to 0

// Protect shared variables from simultaneous access by multiple threads
std::mutex counterMutex;
std::mutex histogramMutex;

// Collatz sequences can exceed 2.1 billion (32-bit int limit). So 704,511 reaches 56.99 billion, therefore use bigger 64-bit long long. 
int stoppingTime(long long num) {
    int st = 0;
    while(num > 1) {
        num = (num % 2 == 0) ? (num/2) : ((3*num)+1);
        st++;
    }
    return st;
}

// Worker thread logic
void worker() {
    while(true) {
        unsigned int current = 0;

        // Claim the next number from the global COUNTER
        if(lock) counterMutex.lock();
        if(COUNTER <= n) {
            current = COUNTER++;
        }
        if(lock) counterMutex.unlock();

        if(current == 0) break; // Exit loop when all work up to N is complete

        int st = stoppingTime(current);

        // Safely record stopping time in global histogram
        if(lock) histogramMutex.lock();
        if(st <= 1000) {
            histogram[st]++;
        }
        if(lock) histogramMutex.unlock();
    }
}

int main(int argc, char *argv[]) {
    // Parse command-line args
    if(argc < 3) {
        std::cerr << "Must use " << argv[0] << " <N> <T> [-nolock]\n";
        return 1;
    }
    n = std::atoi(argv[1]);
    t = std::atoi(argv[2]);
    if (argc >= 4 && std::string(argv[3]) == "-nolock") {
        lock = false;
    }

    // Start execution timer
    struct timespec start, finish;
    clock_gettime(CLOCK_REALTIME, &start);

    // Launch worker threads
    std::vector<std::thread> threads;
    threads.reserve(t);
    for(int i = 0; i < t; i++) {
        threads.emplace_back(worker);
    }

    // Wait for every worker to finish before printing results
    for(int i = 0; i < t; i++) {
        threads[i].join();
    }

    // Stop execution timer
    clock_gettime(CLOCK_REALTIME, &finish);

    // Calculate duration
    long seconds = finish.tv_sec - start.tv_sec;
    long nanoseconds = finish.tv_nsec - start.tv_nsec;
    if(nanoseconds < 0) {seconds--; nanoseconds += 1000000000L;}

    // Print timing info to cerr N,T,time
    std::cerr << n << "," << t << "," << seconds << "." << std::setfill('0') << std::setw(9) << nanoseconds << "\n";

    // Print each stopping time (0-1000) and its frequency in CSV format
    for(int i = 0; i <= 1000; ++i) {
        std::cout << i << "," << histogram[i] << "\n";
    }

    return 0;
}