#include <memory>
#include <cassert>
#include <iostream>


class MainStackAllocator {
    struct Block {
        typedef unsigned char byte;

        byte* begin;
        void* curr;
        size_t sizeLeft;
        
        std::unique_ptr<Block> prevBlock;

        Block(size_t sz, std::unique_ptr<Block> prev):
            begin(new byte[sz]), curr(reinterpret_cast<void*>(begin)), sizeLeft(sz),
            prevBlock(std::move(prev))
        {}

        ~Block() {
            delete[] begin;
        }

        Block() = delete;
        Block(const Block&) = delete;
        Block& operator=(const Block&) = delete;

        template <typename T>
        T* allocate(size_t n, size_t alignment = alignof(T)) {
            if (std::align(alignment, sizeof(T) * n, curr, sizeLeft) == nullptr)
                return nullptr;
            T* ret = reinterpret_cast<T*>(curr);
            curr = reinterpret_cast<void*>(reinterpret_cast<byte*>(curr) + sizeof(T) * n);
            sizeLeft -= sizeof(T) * n;
            return ret;
        }

        size_t capacity() const {
            return (reinterpret_cast<byte*>(curr) - begin) + sizeLeft;
        }
    };

    std::unique_ptr<Block> head;

public:
    MainStackAllocator(): head(nullptr) {}

    MainStackAllocator(const MainStackAllocator&) = delete;
    MainStackAllocator& operator=(const MainStackAllocator&) = delete;
    
    template <typename T>
    T* allocate(size_t n, size_t alignment = alignof(T)) {
        T* ret = nullptr;
        if (head != nullptr && (ret = head->allocate<T>(n, alignment)))
            return ret;
        
        size_t newSize = std::max((head == nullptr ? 0 : head->capacity() * 2), sizeof(T) * n);
        head.reset(new Block(newSize, std::move(head)));
        ret = head->allocate<T>(n, alignment);
        assert(ret != nullptr);
        return ret;
    }
};

template <typename T>
class StackAllocator {
public:
    template <typename U>
    friend class StackAllocator;

    typedef T value_type;

    template <typename U>
    struct rebind {
        typedef StackAllocator<U> other;
    };

private:
    std::shared_ptr<MainStackAllocator> alloc;

public:
    StackAllocator(): alloc(std::make_shared<MainStackAllocator>()) {}

    template <typename U>
    StackAllocator(const StackAllocator<U>& other): alloc(other.alloc) {}
    
    T* allocate(size_t n) {
        T* ret = alloc->allocate<T>(n);
        //std::cout << "alloc(" << n << ") = " << ret << std::endl;
        return ret;
    }

    void deallocate(T* ptr, size_t n) {
        (void)ptr, (void)n;
        //std::cout << "dealloc(" << ptr << ", " << n << ")" << std::endl;
    }
};
