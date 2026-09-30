#include <iostream>
#include <cstdlib>

unsigned int COUNTER = 0;
unsigned int n = 1; //max
unsigned short int t = 1; //threads
bool lock = false; //???

//could just be put into main
void parseArguments(int argc, char *argv[]) {
    //get N, T, and -nolock
    if(argc >= 2) {
        n = atoi(argv[1]);
        t = atoi(argv[2]);
    } 
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

//unneeded
int recordStoppingTime(int nextST) {
    //add result to histogram
    return 0;
}

int main(int argc, char *argv[]) {
    parseArguments(argc, argv);
    
    //initialize histogram array
    unsigned short histogram[1000];
    for(int i=0; i<1000; ++i) {
        histogram[i]=0;
    }

    //find stopping time for every number
    while(COUNTER <= n) {
        int st = stoppingTime(COUNTER++);
        histogram[st]++; //add data point to histogram
    }

    std::cout << "Stoping Time - Frequency\n";
    for(int i=0; i<1000; ++i) {
        if(histogram[i] > 0) {
            std::cout << i << " - " << histogram[i] << std::endl;
        }
    }
    return 0;
}