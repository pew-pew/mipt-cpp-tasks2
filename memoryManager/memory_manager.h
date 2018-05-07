#pragma once

#include <cassert>
#include <cstddef>

class IMemoryManager {
public:
    virtual void* alloc(size_t size) noexcept = 0;
    virtual void free(void* ptr) noexcept = 0;
};

class CMemoryManagerSwitcher {
private:
    IMemoryManager* manager;
    CMemoryManagerSwitcher* previous;

public:
    static CMemoryManagerSwitcher* current;

    CMemoryManagerSwitcher(IMemoryManager* manager): manager(manager), previous(current) {
        current = this;
    }

    ~CMemoryManagerSwitcher() {
        assert(current == this);
        current = previous;
    }

    static void* alloc(std::size_t size);
    static void free(void *ptr);
};

// http://en.cppreference.com/w/cpp/memory/new/operator_delete - Global replacements
//void* operator new(std::size_t size);
//void operator delete(void *ptr) ;
