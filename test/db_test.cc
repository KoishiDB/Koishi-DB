#include "gtest/gtest.h"
#include "db_impl.h"
#include <thread>

namespace koishidb {
    TEST(DB_test, DBBasciTest) {
        DBimpl db;
        for (int i = 0; i < 100; i++) {
            std::string user_key = "user_key" + std::to_string(i);
            std::string value = "user_value" + std::to_string(i);
            db.Put(user_key, value);
        }
        for (int i = 0; i < 100; i++) {
            std::string user_key = "user_key" + std::to_string(i);
            std::string value = "user_value" + std::to_string(i);
            std::string result;
            db.Get(user_key, &result);
            EXPECT_EQ(value, result);
        }
    };

    TEST(DB_test, DBMultiThreadTest) {
        DBimpl db;
        std::vector<std::thread> threads;
        for (int i = 0; i < 1000; i++) {
            std::string user_key = "user_key" + std::to_string(i);
            std::string value = "user_value" + std::to_string(i);
            threads.push_back(std::move(std::thread([&](){
                db.Put(user_key, value);
            })));
        }
        for (int i = 100; i >= 0; i--) {
            std::string user_key = "user_key" + std::to_string(i);
            std::string value = "user_value" + std::to_string(i);
            std::string result;
            threads.push_back(std::move(std::thread([&](){
                db.Get(user_key, &result);
            })));
            EXPECT_EQ(result, value);
        }
        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
    }
}