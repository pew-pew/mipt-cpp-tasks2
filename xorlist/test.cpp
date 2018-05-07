#include <gtest/gtest.h>
#include "xorlist.h"
#include <list>
#include <random>
#include <iostream>

TEST(XorList, stress) {
    std::mt19937 mt; //(std::random_device());
    std::uniform_int_distribution<int> intGen(0, 10);
    auto chance = [&mt](float prob) -> bool { return std::generate_canonical<float, 5>(mt) < prob; };

    std::list<int> stdList;
    XorList<int> xorList;
    for (int iter = 0; iter < 1000; iter++) {
        if (chance(0.7)) {
            int val = intGen(mt);
            
            if (chance(0.5)) {
                if (chance(0.5)) {
                    stdList.push_back(val);
                    xorList.push_back(val);
                } else {
                    stdList.push_front(val);
                    xorList.push_front(val);
                }
            } else {
                size_t index = std::uniform_int_distribution<int>(0, stdList.size())(mt);
                auto stdIt = std::next(stdList.begin(), index);
                auto xorIt = std::next(xorList.begin(), index);
                stdList.insert(stdIt, val);
                xorList.insert_before(xorIt, val);
            }
        } else if (stdList.size()) {
            if (chance(0.5)) {
                if (chance(0.5)) {
                    stdList.pop_back();
                    xorList.pop_back();
                } else {
                    stdList.pop_front();
                    xorList.pop_front();
                }
            } else {
                size_t index = std::uniform_int_distribution<int>(0, stdList.size() - 1)(mt);
                auto stdIt = std::next(stdList.begin(), index);
                auto xorIt = std::next(xorList.begin(), index);
                stdList.erase(stdIt);
                xorList.erase(xorIt);
            }
        }

        ASSERT_EQ(stdList.size(), xorList.size()) << " at iteration " << iter;
        auto stdIt = stdList.begin();
        auto xorIt = xorList.begin();
        for (size_t i = 0; i < stdList.size(); ++i, ++stdIt, ++xorIt) {
            ASSERT_EQ(*stdIt, *xorIt) << "at index " << i << " at iteration " << iter;
            ASSERT_TRUE(xorIt != xorList.end()) << " at iteration " << iter;
        }
        ASSERT_TRUE(xorIt == xorList.end()) << " at iteration " << iter;
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
