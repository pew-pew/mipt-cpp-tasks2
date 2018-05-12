#include <type_traits>
#include <utility>
#include <cstddef>


template <typename...>
class Tuple;

template <typename Tup, typename Target>
using matchHead = std::is_same<decltype(std::decay<Tup>::type::head), Target>;

template <typename Tup>
using empty = std::is_same<typename std::decay<Tup>::type, Tuple<>>;

template <typename T, typename... TailTs>
class Tuple<T, TailTs...> {
private:
    template <typename Head1, typename... Tail1, typename... Body2>
    friend Tuple<Head1, Tail1..., Body2...> tupleCatTwo(Tuple<Head1, Tail1...> tup1, Tuple<Body2...> tup2);

    template <typename... Body2>
    friend Tuple<Body2...> tupleCatTwo(Tuple<>, Tuple<Body2...> tup2);

    template <size_t i, typename Tup, typename std::enable_if<(i > 0), void*>::type>
    friend decltype(auto) get(Tup&& t);

    template <size_t i, typename Tup, typename std::enable_if<(i == 0), void*>::type>
    friend decltype(auto) get(Tup&& t);

    template <typename TT, typename Tup, typename std::enable_if<matchHead<Tup, TT>::value, void*>::type>
    friend decltype(auto) get(Tup&& t);

    template <typename TT, typename Tup, typename std::enable_if<!matchHead<Tup, TT>::value && !empty<Tup>::value, void*>::type>
    friend decltype(auto) get(Tup&& t);

    template <typename TT, typename Tup, typename std::enable_if<empty<Tup>::value, void*>::type>
    friend decltype(auto) get(Tup&&);


    T head;
    Tuple<TailTs...> tail;

public:
    Tuple(): head() {};

    Tuple(const Tuple&) = default;
    Tuple(Tuple&&) = default;

    template <typename THead, typename... TailUs>
    explicit Tuple(THead&& head, TailUs&&... tail):
            head(std::forward<THead>(head)),
            tail(std::forward<TailUs>(tail)...)
    {}


    Tuple& operator=(const Tuple&) = default;
    Tuple& operator=(Tuple&&) = default;

    void swap(Tuple& other) {
        std::swap(head, other.head);
        tail.swap(other.tail);
    }


    bool operator<(const Tuple& other) const {
        if (head != other.head)
            return head < other.head;
        return tail < other.tail;
    }

    bool operator>(const Tuple& other) const { return other < (*this); }
    bool operator<=(const Tuple& other) const { return !operator>(other); }
    bool operator>=(const Tuple& other) const { return !operator<(other); }

    bool operator==(const Tuple& other) const {
        return head == other.head && tail == other.tail;
    }

    bool operator!=(const Tuple& other) const { return !operator==(other); };
};


template <>
class Tuple<> {
public:
    void swap(Tuple&) {}

    bool operator<(const Tuple&) const {
        return false;
    }

    bool operator>(const Tuple& other) const { return other < (*this); }
    bool operator<=(const Tuple& other) const { return !operator>(other); }
    bool operator>=(const Tuple& other) const { return !operator<(other); }

    bool operator==(const Tuple&) const {
        return true;
    }

    bool operator!=(const Tuple& other) const { return !operator==(other); };
};


template <typename... Ts>
Tuple<Ts...> makeTuple(Ts&&... args) {
    return Tuple<Ts...>(std::forward<Ts>(args)...);
}


template <size_t i, typename Tup, typename std::enable_if<(i > 0), void*>::type = nullptr>
decltype(auto) get(Tup&& t) {
    return get<i - 1>(std::forward<Tup>(t).tail);
}

template <size_t i, typename Tup, typename std::enable_if<(i == 0), void*>::type = nullptr>
decltype(auto) get(Tup&& t) {
    return (std::forward<Tup>(t).head);
}


template <typename T, typename Tup, typename std::enable_if<matchHead<Tup, T>::value, void*>::type = nullptr>
decltype(auto) get(Tup&& t) {
    return (std::forward<Tup>(t).head);
}

template <typename T, typename Tup, typename std::enable_if<!matchHead<Tup, T>::value && !empty<Tup>::value, void*>::type = nullptr>
decltype(auto) get(Tup&& t) {
    return get<T>(std::forward<Tup>(t).tail);
}

template <typename T, typename Tup, typename std::enable_if<empty<Tup>::value, void*>::type = nullptr>
decltype(auto) get(Tup&&) {
    static_assert(!empty<Tup>::value, "tuple doesn't contain specified type");
}


template <typename Head1, typename... Tail1, typename... Body2>
Tuple<Head1, Tail1..., Body2...> tupleCatTwo(Tuple<Head1, Tail1...> tup1, Tuple<Body2...> tup2) {
    return Tuple<Head1, Tail1..., Body2...>(std::move(tup1).head, tupleCatTwo(std::move(tup1).tail, std::move(tup2)));
}

template <typename... Body2>
Tuple<Body2...> tupleCatTwo(Tuple<>, Tuple<Body2...> tup2) {
    return std::move(tup2);
}

Tuple<> tupleCat() {
    return Tuple<>();
}

template <typename Head, typename... Tail>
auto tupleCat(Head&& head, Tail&&... tail) {
    return tupleCatTwo(std::forward<Head>(head), tupleCat(std::forward<Tail>(tail)...));
}
