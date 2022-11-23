#ifndef KOISHIDB_SRC_INCLUDE_TABLE_BUILDER_H
#define KOISHIDB_SRC_INCLUDE_TABLE_BUILDER_H

#include <cstdint>

#include "common/status.h"
#include "disk/block_builder.h"
#include "disk/format.h"
namespace koishidb {
    // we don't need Option->
    struct Option;
    class WritableFile;
    class TableBuilder {
    public:
        TableBuilder() = delete;
        TableBuilder& operator=(const TableBuilder& that) = delete;

        ~TableBuilder();

        TableBuilder(const Option* opt, WritableFile* file);

        // add the key and value to the Table
        void Add(const Slice& key, const Slice& value);

        void Flush();

        // finish the build, should be called when finish and abandon not have been called;
        Status Finish();

        // abandon the build, should be called when finish and abandon not have been called;
        void Abandon();

        // return the num entries of the total entries
        uint64_t NumEntries() const;

        // return the generated file size
        uint64_t FileSize() const;

        Status status() const;
      private:
        bool ok() const { return status().ok(); }

        void WriteBlock(BlockBuilder* block_builder, BlockHandle* block_handle);

        struct Rep;
        Rep* rep_;
        // Data Block, index block->
        // index block-> key block->smallest, value-> block offset
    };
};
#endif