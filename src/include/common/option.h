#ifndef KOISHIDB_SRC_INCLUDE_COMMON_OPTION_H
#define KOISHIDB_SRC_INCLUDE_COMMON_OPTION_H

namespace koishidb {
    // forward declarations
    // Some Options that
    class Comparator;
    // currently option owned the Comparator and should delete it
    struct Option {
        Option();
        Option(const Comparator* opt);
        const Comparator* cmp; // to compare the last key
        ~Option() {
          if (cmp != nullptr) {
            delete cmp;
          }
        }
    };

};

#endif