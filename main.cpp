#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <vector> // Used to store worker threads

unsigned int COUNTER = 1;
unsigned int n = 1; //max
unsigned short int t = 1; //threads
bool lock = false; // TODO: change to true by default; -nolock should disable synchronization

//global so all worker threads can access the same histogram
unsigned short histogram[1001] = {0}; //this trick automatically initializes all values to 0, same as the previous loop implementation

// Protect shared variables from simultaneous access by multiple threads
std::mutex counterMutex;
std::mutex histogramMutex;

//could just be put into main
void parseArguments(int argc, char *argv[]) {
    //get N, T, and need to add -nolock
    if(argc >= 2) {
        n = atoi(argv[1]);
        t = atoi(argv[2]);
    }

    // TODO: check for optional -nolock argument
    // TODO: set lock true by default, false when -nolock is given
    // IMPORTANT: currently lock is always false, so neither mutex is being used
    //if(argc >= 3) {lock = std::string(argv[3]) != "-nolock";}
}

int stoppingTime(int num) {
    int st = 0;
    //std::cout << num;
    while(num > 1) {
        st++;
        if(num%2==0){
            num = num/2;
        } else {
            num = (num*3)+1;
        }
        //std::cout << " > " << num;
    }
    //std::cout << std::endl;
    return st;
}

void recordStoppingTime(int nextST) {
    // Lock prevents threads from updating the same histogram entry simultaneously
    if(lock) {
        histogramMutex.lock();
    }

    // TODO: ensure nextST is within the histogram's 0-1000 range
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

    // TODO: start timer here

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

    // TODO: stop timer here

    // TODO: print timing information to cerr N,T,time

    // TODO: doc requires CSV output (stoppingTime,frequency) without a heading
    std::cout << "Stoping Time - Frequency\n";

    for(int i=0; i<=1000; ++i) {
        // TODO: The doc requires printing all entries, including zero frequencies
        if(histogram[i] > 0) {
            std::cout << i << " - " << histogram[i] << std::endl;
        }
    }

    // DOUBLY IMPORTANT: successful compilation does not guarantee correct synchronization.
    // We should test locked and -nolock runs and compare their histogram outputs.
    // Unlocked mode will intentionally allow for race conditions, so its results are unreliable.

    return 0;
}