#ifndef KOISHIDB_SRC_INCLUDE_COMMON_OPTION_H
#define KOISHIDB_SRC_INCLUDE_COMMON_OPTION_H

namespace koishidb {
    // forward declarations
    class Comparator;
    struct Option {
        Option();
        const Comparator* cmp; // to compare the last key
    };

};

#endif