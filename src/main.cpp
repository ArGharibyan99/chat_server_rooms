#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include <unistd.h>     // getpid
#include <sys/syscall.h> // SYS_gettid
#include "my_class.h"

using namespace std::chrono;
typedef unsigned long long ull;

ull OddSum = 0;
ull EvenSum = 0;

pid_t gettid_cpp()
{
    return syscall(SYS_gettid);   // linux only
}

void findEven(ull start, ull end) {
    std::cout << "process pid = " << getpid()
              << ", thread tid = " << gettid_cpp()
              << std::endl;
	for(ull i = 0; i <= end; ++i) {
		if((i & 1) == 0) {
			EvenSum += i;
		}
	}
}

void findOdd(ull start, ull end) {
    std::cout << "process pid = " << getpid()
              << ", thread tid = " << gettid_cpp()
              << std::endl;
	for(ull i = 0; i <= end; ++i) {
		if((i & 1) == 1) {
			OddSum += i;
		}
	}
}

int main() {
    ull start = 0, end = 1900000000;

    auto startTime = high_resolution_clock::now();

    std::thread t1(findEven, start, end);
    std::thread t2(findOdd, start, end);

    t1.join();
    t2.join();

    // findOdd(start, end);
    // findEven(start, end);

    auto stopTime = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stopTime- startTime);
    
    std::cout << "OddSum : " << OddSum << std::endl;
    std::cout << "EvenSum : " << EvenSum << std::endl;
    std::cout << "Sec : " << duration.count()/1000000 << std::endl;

    return 0;
}

