#ifndef KOISHIDB_SRC_INCLUDE_TABLE_BUILDER_H
#define KOISHIDB_SRC_INCLUDE_TABLE_BUILDER_H

namespace koishidb {
    class TableBuilder {
    public:
        TableBuilder() = default;
        TableBuilder& operator=(const TableBuilder& that) = default;

        // Data Block, index block->
        // index block-> key block->smallest, value-> block offset
    };
};
#endif