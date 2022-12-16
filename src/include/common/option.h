#ifndef KOISHIDB_SRC_INCLUDE_COMMON_OPTION_H
#define KOISHIDB_SRC_INCLUDE_COMMON_OPTION_H

namespace koishidb {
// forward declarations
// Some Options that
class Comparator;
// currently option owned the Comparator and should delete it
struct Option {
  Option();

  // we should let the Option delete the Comparator
  Option(const Comparator* opt);
  const Comparator* cmp;  // to compare the last key
  ~Option();
};

};  // namespace koishidb

#endif