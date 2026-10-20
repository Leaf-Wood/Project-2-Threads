#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <vector> // Used to store worker threads
#include <string> 
#include <iomanip> // formating for timer

unsigned int COUNTER = 1;
unsigned int n = 1;
unsigned short int t = 1; // Num of threads
bool lock = true; // False enables race conditions. 
unsigned int histogram[1001] = {0}; // Initializes all values to 0

// Protect shared variables from simultaneous access by multiple threads
std::mutex counterMutex;
std::mutex histogramMutex;

void parseArguments(int argc, char *argv[]) {
    // Read N and T from the command line
    if (argc >= 3) {
        n = atoi(argv[1]);
        t = atoi(argv[2]);
    }

    // Disables synchronization if -nolock is provided
    if (argc == 4 && std::string(argv[3]) == "-nolock") {
        lock = false;
    }
}

// Collatz sequences can exceed 2.1 billion (32-bit int limit).
// So 704,511 reaches 56.99 billion, therefore use bigger 64-bit long long. Try saying that fast three times over. 
int stoppingTime(long long num) {
    int st = 0;
    while(num > 1) {
        st++;

        if(num%2==0){
            num = num/2;
        } else {
            num = (num*3)+1;
        }
    }
    return st;
}

void recordStoppingTime(int nextST) {
    // Ensure the stopping time is within the histogram's range
    if (nextST < 0 || nextST > 1000) {
        std::cerr << "Error: Stopping time out of range." << std::endl;
        std::abort();
    }

    // Lock prevents threads from updating the same entry simultaneously
    if(lock) {
        histogramMutex.lock();
    }

    histogram[nextST]++;

    if(lock) {
        histogramMutex.unlock();
    }
}

/* This function is run by each worker thread. It claims a number from the global counter, calculates its stopping time, and records the result in the histogram.
*/
void worker() {
    while(true) {
        int currentNumber;

        if(lock) {
            counterMutex.lock();
        }

        // Check and claim a number while holding the same lock to prevent duplicate work
        if(COUNTER > n) {
            if(lock) {
                counterMutex.unlock();
            }
            break;
        }

        currentNumber = COUNTER++;

        // Release the lock BEFORE computation so other threads can claim work.
        // Holding it during stoppingTime() would prevent other threads from claiming numbers.
        if(lock) {
            counterMutex.unlock();
        }

        int st = stoppingTime(currentNumber);
        recordStoppingTime(st);
    }
}

int main(int argc, char *argv[]) {
    parseArguments(argc, argv);

    // start timer here
    struct timespec start, finish;
    clock_gettime(CLOCK_REALTIME, &start);

    // Store all worker threads so main can wait for them
    std::vector<std::thread> threads;

    // Create t threads, all running the same worker function
    for(int i = 0; i < t; i++) {
        threads.push_back(std::thread(worker));
    }

    // Wait for every worker to finish before printing results
    for(int i = 0; i < t; i++) {
        threads[i].join();
    }

    // stop timer here
    clock_gettime(CLOCK_REALTIME, &finish);
    long seconds = finish.tv_sec - start.tv_sec;
    long nanoseconds = finish.tv_nsec - start.tv_nsec;
    if(nanoseconds < 0) {seconds--; nanoseconds += 1000000000L;}

    //  print timing information to cerr N,T,time
    std::cerr << n << "," << t << "," << seconds << "." << std::setfill('0') << std::setw(9) << nanoseconds << std::endl;

    /* // Print each stopping time (0-1000) and its frequency in CSV format
    for(int i = 0; i <= 1000; ++i) {
        if(histogram[i]!=0) {
            std::cout << i << "," << histogram[i] << std::endl;
        }
    }*/

    return 0;
}