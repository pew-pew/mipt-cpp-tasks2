#pragma once

#include <cstddef>
#include <cstdlib>
#include <new>
#include "memory_manager.h"

template <typename AllocStrategy>
class CAllocatedOn {
public:
    static void* operator new(size_t sz) {
        return AllocStrategy::Alloc(sz);
    }

    static void* operator new[](size_t sz) {
        return AllocStrategy::Alloc(sz);
    }

    static void operator delete(void* ptr) {
        AllocStrategy::Free(ptr);
    }

    static void operator delete[](void* ptr) {
        AllocStrategy::Free(ptr);
    }
};

class RuntimeHeap {
public:
    static void* Alloc(size_t sz) {
        void* ptr = malloc(sz);
        if (!ptr)
            throw std::bad_alloc();
        return malloc(sz);
    }

    static void Free(void* ptr) {
        free(ptr);
    }
};

class CurrentMemoryManager {
public:
    static void *Alloc(size_t sz) {
        return CMemoryManagerSwitcher::alloc(sz);
    }

    static void Free(void* ptr) {
        return CMemoryManagerSwitcher::free(ptr);
    }
};
