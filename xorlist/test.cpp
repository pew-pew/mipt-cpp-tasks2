#include <iostream>
#include "stack_allocator.h"
#include "xorlist.h"
#include <iterator>
#include <type_traits>
#include <vector>

template class XorList<int>;

int main_() {
    /*
    XorList<int> list;
    list.push_back(5);
    list.push_back(10);
    
    list.insert_before(std::next(list.begin()), 3);
    for (auto it = list.begin(); it != list.end(); it++)
        std::cout << *it << " ";
    std::cout << std::endl;
    
    list.pop_back();
    */

    XorList<int, StackAllocator<int>> list;

    for (int i = 0; i < 10; i++) {
        if (i % 2) list.push_back(i);
        else list.push_front(i);

        for (auto it = list.begin(); it != list.end(); it++)
            std::cout << *it << " ";
        std::cout << std::endl;
    }
    
    {
        auto l2 = list;
        for (auto it = l2.begin(); it != l2.end(); it++)
            std::cout << *it << " ";
        std::cout << std::endl;
    }
    
    for (auto it = list.begin(); it != list.end(); it++)
        std::cout << *it << " ";
    std::cout << std::endl;
    return 0;
}

using vec = std::vector<int>;
using List = XorList<vec>;
int main() {
    List list;
    for (int i = 0; i < 5; i++) {
        if (i % 2 == 0) list.push_back(vec());
        else list.push_front(vec());

        for (auto it = list.begin(); it != list.end(); it++)
            it->push_back(i);
    }

    for (auto it = list.begin(); it != list.end(); it++) {
        for (auto x : *it)
            std::cout << x << " ";
        std::cout << std::endl;
    }
}
