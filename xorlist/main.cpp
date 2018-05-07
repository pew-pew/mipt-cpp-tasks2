#include "stack_allocator.h"
#include <chrono>
#include <list>
#include <cstdio>

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

template<typename intAlloc>
int measureMs() {
    auto start = steady_clock::now();
    
    std::list<int, intAlloc> list;

    for (int i = 0; i < 30000000; i++) {
        int x = rand() % 100;
        if (x < 33 && !list.empty())
            list.pop_front();
        else
            list.push_back(5);
    }

    auto finish = steady_clock::now();
    auto ms = duration_cast<milliseconds>(finish - start);
    return ms.count();
}

int main() {
    for (int i = 0; i < 2; i++) {
        std::cout << "std::allocator<int>: ";
        std::cout << measureMs<std::allocator<int>>() << " ms" << std::endl;
        
        std::cout << "StackAllocator<int>: ";
        std::cout << measureMs<StackAllocator<int>>() << " ms" << std::endl;
    }
}
