#include "memory/skiplist.h"
#include "memory/memtable.h"
#include "util/util.h"
#include "util/comparator.h"
#include "gtest/gtest.h"

#include <set>

class DoubleComparator {
public:
    int Compare(const double& a, const double& b) const {
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
        DoubleComparator cmp;
        SkipList<double, DoubleComparator> skiplist(&cmp);
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
        DoubleComparator cmp;
        SkipList<double, DoubleComparator> skiplist(&cmp);
        std::set<double> right;
        for (int i = 0; i < 10000; ++i) {
            double x = rand();
            bool ok = right.insert(x).second;
            if (ok) {
                skiplist.Insert(x);
            }
        }

        std::set<double> result;
        SkipList<double, DoubleComparator>::Iterator iter(&skiplist);
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


    // this test still has some question, need to help
    TEST(SkipList_test, SkipListSliceTest) {
        SkipList<Slice, MemtableKeyComparator> skipList(new MemtableKeyComparator());
        Slice memtable_key1 = CreateMemtableKey("user_key1", 1, "value1", KeyType::kTypeValue);
        skipList.Insert(memtable_key1);
        Slice memtable_key2 = CreateMemtableKey("user_key2", 2, "valu2", KeyType::kTypeValue);
        Slice result;
        // Test1, find the memtable_key1
        bool flag = skipList.FindFirstGreaterOrEqual(CreateMemtableKey("user_key1", 1, "", KeyType::kTypeValue), &result);
        EXPECT_EQ(flag, true);
        EXPECT_EQ(result, memtable_key1);

        // Test2, can't find the user_key3
        flag = skipList.FindFirstGreaterOrEqual(CreateMemtableKey("user_key3", 1, "", KeyType::kTypeValue), &result);
        EXPECT_EQ(flag, false);

        // Test3, find the first larger
        flag = skipList.FindFirstGreaterOrEqual(CreateMemtableKey("user_key1", 0, "", KeyType::kTypeValue), &result);
        EXPECT_EQ(flag, true);
        EXPECT_EQ(result, memtable_key2);

        // Test4, insert the delete key and find the newest one;
        Slice memtable_key3 = CreateMemtableKey("user_key1", 3, "", KeyType::kTypeDeletion);
        skipList.Insert(memtable_key3);
        flag = skipList.FindFirstGreaterOrEqual(CreateMemtableKey("user_key1", 4, "", KeyType::kTypeValue), &result);
        EXPECT_EQ(flag, true);
        EXPECT_EQ(result, memtable_key3);

        // Test5, can't find the user_key2 but with snapshot 1;
        flag = skipList.FindFirstGreaterOrEqual(CreateMemtableKey("user_key2", 1, "", KeyType::kTypeValue), &result);
        EXPECT_EQ(flag, false);
    }
};