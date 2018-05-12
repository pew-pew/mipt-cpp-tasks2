#include "tuple.h"
#include <gtest/gtest.h>
#include <type_traits>

template <typename T, typename U>
constexpr bool is_same = std::is_same<T, U>::value;

#define ASSERT_IS(expr, type) static_assert(is_same<decltype((expr)), type>, "")

typedef Tuple<int, float, int> Tuple_int_float_int;
TEST(Tuple, Constructor) {
    Tuple_int_float_int t;
    EXPECT_EQ(t, Tuple_int_float_int(0, 0.0f, 0));
}

TEST(Tuple, Comparison) {
    EXPECT_EQ(Tuple_int_float_int(1, 3.14f, 2), Tuple_int_float_int(1, 3.14f, 2));
    EXPECT_NE(Tuple_int_float_int(1, 3.15f, 2), Tuple_int_float_int(1, 3.14f, 2));
    EXPECT_LT(Tuple_int_float_int(0, 3.14f, 2), Tuple_int_float_int(1, 3.14f, 2));
    EXPECT_LT(Tuple_int_float_int(1, 3.13f, 2), Tuple_int_float_int(1, 3.14f, 2));
    EXPECT_GT(Tuple_int_float_int(2, 3.13f, 2), Tuple_int_float_int(1, 3.14f, 2));
}

TEST(Tuple, Get) {
    Tuple<int, float, int> t(1, 3.14f, 3);
    const Tuple<int, float, int> &const_t = t;

    EXPECT_EQ(get<0>(t), 1);
    EXPECT_EQ(get<1>(t), 3.14f);
    EXPECT_EQ(get<2>(t), 3);

    EXPECT_EQ(get<0>(const_t), 1);
    EXPECT_EQ(get<1>(const_t), 3.14f);
    EXPECT_EQ(get<2>(const_t), 3);

    EXPECT_EQ(get<0>(std::move(t)), 1);
    EXPECT_EQ(get<1>(std::move(t)), 3.14f);
    EXPECT_EQ(get<2>(std::move(t)), 3);

    EXPECT_EQ(get<int>(t), 1);
    EXPECT_EQ(get<float>(t), 3.14f);


    ASSERT_IS(get<0>(t), int &);
    ASSERT_IS(get<0>(const_t), const int&);
    ASSERT_IS(get<0>(std::move(t)), int &&);

    ASSERT_IS(get<1>(t), float &);
    ASSERT_IS(get<1>(const_t), const float&);
    ASSERT_IS(get<1>(std::move(t)), float &&);

    ASSERT_IS(get<int>(t), int &);
    ASSERT_IS(get<int>(const_t), const int&);
    ASSERT_IS(get<int>(std::move(t)), int &&);

    ASSERT_IS(get<float>(t), float &);
    ASSERT_IS(get<float>(const_t), const float&);
    ASSERT_IS(get<float>(std::move(t)), float &&);
}

TEST(Tuple, Concatenate) {
    typedef Tuple<int, float, char> Tuple_ifc;
    auto x = tupleCat(Tuple<int>(5), Tuple<>(), Tuple<float>(7), Tuple<char>('#'));
    ASSERT_IS(x, Tuple_ifc&);
    EXPECT_EQ(x, Tuple_ifc(5, 7, '#'));
}

struct NoCopy {
    NoCopy() = default;
    NoCopy(const NoCopy&) = delete;
    NoCopy(NoCopy&&) = default;

    NoCopy& operator=(const NoCopy&) = delete;
    NoCopy& operator=(NoCopy&&) = default;
};

TEST(Tuple, Move) {
    NoCopy x;
    (void)(Tuple<NoCopy, NoCopy>(std::move(x), NoCopy()));
    (void)(tupleCat(Tuple<NoCopy>(), Tuple<NoCopy>()));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
