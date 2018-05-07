#include "memory_manager.h"

#include <cstdlib>
#include <cstddef>
#include <type_traits>
#include <new>
#include <iostream>


class Logger {
    static std::string _prefix;

public:
    static Logger log;
    class prefix {
        std::string _oldPrefix;
    public:
        prefix(std::string part): _oldPrefix(std::move(_prefix)) {
            _prefix = _oldPrefix + part;
        }

        ~prefix() {
            _prefix = std::move(_oldPrefix);
        }
    };
    template <typename T>
    std::ostream& operator<<(T val) {
        std::cout << _prefix;
        return std::cout << val;
    }
};
std::string Logger::_prefix = "";
Logger Logger::log;

struct alignas(alignof(std::max_align_t)) InfoBlock {
    CMemoryManagerSwitcher* mm;
};

void* CMemoryManagerSwitcher::alloc(std::size_t size) {
    Logger::log << "alloc(" << size << ") at " << current << std::endl;
    size_t allocSz = sizeof(InfoBlock) + size;

    InfoBlock *block;
    {
        Logger::prefix p("| ");
        CMemoryManagerSwitcher *currentCopy = current;
        if (current)
            current = current->previous;
        block = reinterpret_cast<InfoBlock*>(currentCopy && currentCopy->manager
                                             ? currentCopy->manager->alloc(allocSz) : malloc(allocSz));
        current = currentCopy;

        if (!block)
            throw std::bad_alloc();
        block->mm = current;
    }
    Logger::log << " = " << (block + 1) << std::endl;
    return reinterpret_cast<void*>(block + 1);
}

void CMemoryManagerSwitcher::free(void *ptr) {
    Logger::log << "free(" << ptr << ") at " << current << std::endl;
    Logger::prefix p("| ");
    if (!ptr)
        return;
    InfoBlock* block = reinterpret_cast<InfoBlock*>(ptr) - 1;
    ptr = reinterpret_cast<void*>(block);
    CMemoryManagerSwitcher* allocatedWith = block->mm;
    if (allocatedWith && allocatedWith->manager)
        allocatedWith->manager->free(ptr);
    else
        std::free(ptr);
}

CMemoryManagerSwitcher* CMemoryManagerSwitcher::current = nullptr;

void* operator new(std::size_t size) {
    return CMemoryManagerSwitcher::alloc(size);
}

void operator delete(void *ptr) {
    return CMemoryManagerSwitcher::free(ptr);
}


