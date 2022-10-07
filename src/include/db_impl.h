#ifndef KOISHIDB_SRC_INCLUDE_DB_IMPL_H
#define KOISHIDB_SRC_INCLUDE_DB_IMPL_H

#include "db.h"
#include "memory/memtable.h"
#include <deque>
#include <shared_mutex>
#include <mutex>



namespace koishidb {

    class DBimpl: DB {
    public:
        void Put(const Slice& key, const Slice& value);

        bool Get(const Slice& key, std::string* value);

        void Delete(const Slice& key);

        void Write(WriteBatch* update);
    private:
        struct Writer; // declare here
        std::deque<Writer* > writers_;
        std::shared_mutex rwlock_;
        Memtable memtable_;
    };
};

#endif