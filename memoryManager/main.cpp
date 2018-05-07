#include <iostream>
#include "memory_manager.h"
#include "stack_allocator.h"
#include "allocated_on.h"

struct XXX: public CAllocatedOn<CurrentMemoryManager> {
    int a = 0;
    int b = 0;
    int c = 0;
};

int main() {
    StackMemoryManager m;
    CMemoryManagerSwitcher tmp(&m);
    StackMemoryManager m2;
    CMemoryManagerSwitcher tmp2(&m2);
    std::cout << "--\n";
    int* a = new int[100];
    delete[] a;
    std::cout << "--\n";
    XXX* x = new XXX;
    delete x;
    std::cout << "--\n";
}
