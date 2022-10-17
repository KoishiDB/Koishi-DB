#include "memory/skiplist.h"
#include "gtest/gtest.h"

#include <set>

class DoubleCompare {
public:
    int operator()(const double& a, const double& b) const {
        if (a < b) {
            return -1;
        } else if (a == b) {
            return 0;
        } else {
            return 1;
        }
    }
};

namespace koishidb {
    TEST(SkipList_test, SkipListRandomdTest) {
        srand(0);
        DoubleCompare cmp;
        SkipList<double, DoubleCompare> skiplist(cmp);
        std::set<double> right;
        for (int i = 0; i < 10000; ++i) {
            double x = rand();
            bool ok = right.insert(x).second;
            if (ok) {
                skiplist.Insert(x);
            }
        }
        for (int i = 0; i < 10000; ++i) {
            double x = rand();
            auto a = right.lower_bound(x);
            double b;
            bool flag = skiplist.FindFirstGreaterOrEqual(x, &b);
            EXPECT_EQ(flag, a != right.end());
            if (flag == true) {
                EXPECT_EQ(*a, b);
            }
        }
    }

    TEST(SkipList_test, SkipListIteratorTest1) {
        srand(0);
        DoubleCompare cmp;
        SkipList<double, DoubleCompare> skiplist(cmp);
        std::set<double> right;
        for (int i = 0; i < 10000; ++i) {
            double x = rand();
            bool ok = right.insert(x).second;
            if (ok) {
                skiplist.Insert(x);
            }
        }

        std::set<double> result;
        SkipList<double, DoubleCompare>::Iterator iter(skiplist);
        while (iter.Valid()) {
            result.insert(*iter);
            ++iter;
        }

        if (right == result) {
            EXPECT_EQ(1, 1);
        } else {
            EXPECT_EQ(0, 1);
        }
    }
};