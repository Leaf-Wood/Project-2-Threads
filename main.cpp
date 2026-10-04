#include <iostream>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <vector> //just in case, I think using vector is a good idea for storing threads

unsigned int COUNTER = 0;
unsigned int n = 1; //max
unsigned short int t = 1; //threads
bool lock = false; //nice

//global so all worker threads can access the same histogram
unsigned short histogram[1001] = {0}; //this trick automatically initializes all values to 0, same as the previous loop implementation

// TODO: create mutex for COUNTER and histogram

//could just be put into main
void parseArguments(int argc, char *argv[]) {
    //get N, T, and need to add -nolock
    if(argc >= 2) {
        n = atoi(argv[1]);
        t = atoi(argv[2]);
    }

    // TODO: check for optional -nolock argument
    // TODO: set lock true by default, false when -nolock is given
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
    //add result to shared histogram

    // TODO: if locking is enabled through -nolock I think, protect this histogram update
    // with the histogram mutex

    histogram[nextST]++;
}

void worker() {
    //each thread will run this same function

    while(COUNTER <= n) {

        // TODO: protect access to COUNTER with the counter mutex
        // Each worker needs to safely claim ONE number and increment
        // COUNTER so another worker does not claim the same number.
        int currentNumber = COUNTER++;

        //worker calculates the stopping time for the number it claimed
        int st = stoppingTime(currentNumber);

        //worker records its result in the shared histogram
        recordStoppingTime(st);
    }
}

int main(int argc, char *argv[]) {
    parseArguments(argc, argv);

    // TODO: start timer here

    // TODO: instead of calling worker() directly,
    // create t threads and have every thread run worker()
    worker();

    // TODO: join all worker threads here
    // Main must wait until every worker is finished.

    // TODO: stop timer here

    // TODO: print timing information to cerr N,T,time

    std::cout << "Stoping Time - Frequency\n";

    for(int i=0; i<=1000; ++i) {
        if(histogram[i] > 0) {
            std::cout << i << " - " << histogram[i] << std::endl;
        }
    }

    return 0;
}