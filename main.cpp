#include <iostream>

int stoppingTime(int n) {
    int st = 0;
    std::cout << n;
    while(n != 1) {
        st++;
        if(n%2==0){
            n = n/2;
        } else {
            n = (n*3)+1;
        }
        std::cout << " > " << n;
    }
    std::cout << std::endl;
    return st;
}

int main() {
    int n;
    std::cout << "Enter number: ";
    std::cin >> n;
    
    int st = stoppingTime(n);
    std::cout << "Stopping time is " << st << std::endl;

    return 0;
}