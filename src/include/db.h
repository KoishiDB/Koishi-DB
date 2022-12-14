#ifndef KOISHIDB_SRC_INCLUDE_DB_H
#define KOISHIDB_SRC_INCLUDE_DB_H

#include <string>
#include "memory/write_batch.h"

namespace koishidb {
    class DB {
    public:
        virtual void Put(const Slice& key, const Slice& value) = 0;

        virtual bool Get(const Slice& key, std::string* value) = 0;

        virtual void Delete(const Slice& key) = 0;

        virtual void Write(WriteBatch* update) = 0;
    };
};
#endif