#ifndef KOISHIDB_SRC_INCLUDE_COMMON_OPTION_H
#define KOISHIDB_SRC_INCLUDE_COMMON_OPTION_H

namespace koishidb {
    // forward declarations
    // Some Options that
    class Comparator;
    struct Option {
        Option();
        Option(const Comparator* opt);
        const Comparator* cmp; // to compare the last key
    };

};

#endif