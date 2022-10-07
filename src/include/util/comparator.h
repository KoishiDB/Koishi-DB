#ifndef KOISHIDB_SRC_INCLUDE_UTIL_COMPARATOR_H
#define KOISHIDB_SRC_INCLUDE_UTIL_COMPARATOR_H

#include "type/slice.h"
#include "util/util.h"
namespace koishidb {
  class Comparator {
  public:

      // 0 means equal, 1 means internal_key1 > internal_key2, -1 means internal_key1 < internal_key2
      // 按照user_key 升序, seq降序排列
      int operator()(Slice& internal_key1, Slice& internal_key2) const {
          Slice *user_key1, *user_key2;
          SequenceNumber  *seq1, *seq2;
          ExtractUserKey(internal_key1, user_key1, seq1);
          ExtractUserKey(internal_key2, user_key2, seq2);

          if (user_key1 == user_key2) {
             if (seq1 < seq2) {
                 return 1;
             } else if (seq1 == seq2) {
                 return 0;
             } else {
                 return -1;
             }
          }
          if (user_key1 < user_key2) {
              return -1;
          } else {
              return 1;
          }
      }
  };
};

#endif