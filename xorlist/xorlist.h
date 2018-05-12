#include <memory>
#include <cassert>

template <typename T, typename Allocator = std::allocator<T>>
class XorList {
    template <typename U>
    static uintptr_t& ptr2int(U *&p) {
        return reinterpret_cast<uintptr_t&>(p);
    }

    struct Node {
        T value;
        uintptr_t nbXor;
        
        template <typename TT>
        Node(TT&& value):
            value(std::forward<TT>(value)), nbXor(0)
        {}
    };
    
    class iterator: public std::iterator<std::bidirectional_iterator_tag, T> {
        friend class XorList;
        Node* curr;
        Node* prev;

    public:
        iterator(): curr(nullptr), prev(nullptr) {}
        iterator(Node* curr, Node* prev): curr(curr), prev(prev) {}

        T& operator*() const {return curr->value;}
        T* operator->() const {return &(curr->value);}
        
        template <Node* iterator::*curr = &iterator::curr, Node* iterator::*prev = &iterator::prev>
        iterator& operator++() {
            Node* next = reinterpret_cast<Node*>((this->*curr)->nbXor ^ ptr2int(this->*prev));
            this->*prev = this->*curr;
            this->*curr = next;
            return *this;
        }
        
        template <iterator& (iterator::*increment)() = &iterator::operator++>
        iterator operator++(int) {
            iterator copy(*this);
            (this->*increment)();
            return copy;
        }

        iterator& operator--() {
            return operator++<&iterator::prev, &iterator::curr>();
        }

        iterator operator--(int) {
            return operator++<&iterator::operator-->(0);
        }

        bool operator==(const iterator& other) const {return curr == other.curr;}
        bool operator!=(const iterator& other) const {return !operator==(other);}
    };

    // toggle between states:
    //      left <=> mid <=> right
    // and
    //      left <=========> right
    //         0 <=> mid <=> 0
    static void toggleInsert(Node* left, Node* mid, Node* right) {
        if (left)  left->nbXor  ^= ptr2int(mid)  ^ ptr2int(right);
        if (mid)   mid->nbXor   ^= ptr2int(left) ^ ptr2int(right);
        if (right) right->nbXor ^= ptr2int(left) ^ ptr2int(mid);
    }

    typedef typename Allocator::template rebind<Node>::other NodeAllocator;

    NodeAllocator alloc;
    Node* first;
    Node* last;
    size_t size_;

public:
    explicit XorList(const Allocator& alloc = Allocator()):
        alloc(alloc), first(nullptr), last(nullptr), size_(0)
    {}
    
    XorList(size_t count, const T& value = T(), const Allocator& alloc = Allocator()): XorList(alloc) {
        for (size_t i = 0; i < count; ++i)
            push_back(value);
    }
    
    XorList(const XorList& other): XorList() {
        for (iterator it = other.begin(); it != other.end(); ++it)
            push_back(*it);
    }

    XorList(XorList&& other): alloc(std::move(other.alloc)), first(other.first), last(other.last), size_(other.size_) {
        other.first = nullptr;
        other.last = nullptr;
        other.size_ = 0;
    }

    ~XorList() {
        while (!empty())
            pop_back();
        assert(first == nullptr && last == nullptr && size_ == 0);
    }

    XorList& operator=(const XorList& other) {
        while (!empty())
            pop_back();
        for (iterator it = other.begin(); it != other.end(); it++)
            push_back(*it);
        return *this;
    }

    XorList& operator=(XorList&& other) {
        std::swap(alloc, other.alloc);
        std::swap(first, other.first);
        std::swap(last, other.last);
        std::swap(size_, other.size_);
        return *this;
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    iterator begin() const {
        return iterator(first, nullptr);
    }

    iterator end() const {
        return iterator(nullptr, last);
    }

    void erase(iterator it) {
        assert(it.curr != nullptr);
        Node *prev = it.prev, *mid = it.curr, *next = (++iterator(it)).curr;
        
        toggleInsert(prev, mid, next);
        if (first == mid) first = next;
        if (last  == mid) last  = prev;
        --size_;
        
        mid->~Node();
        std::allocator_traits<NodeAllocator>::deallocate(alloc, mid, 1);
    }

    void pop_back() {
        erase(--end());
    }

    void pop_front() {
        erase(begin());
    }

    template <typename TT>
    void insert_before(iterator it, TT&& val) {
        Node *prev = it.prev, *next = it.curr;
        
        Node* mid = std::allocator_traits<NodeAllocator>::allocate(alloc, 1);
        new(mid) Node(std::forward<TT>(val));

        toggleInsert(prev, mid, next);
        if (first == next) first = mid;
        if (last  == prev) last  = mid;

        size_++;
    }

    template <typename TT>
    void insert_after(iterator it, TT&& val) {
        insert_before(++it, std::forward<TT>(val));
    }

    template <typename TT>
    void push_back(TT&& val) {
        insert_before(end(), std::forward<TT>(val));
    }
    
    template <typename TT>
    void push_front(TT&& val) {
        insert_before(begin(), std::forward<TT>(val));
    }
};
