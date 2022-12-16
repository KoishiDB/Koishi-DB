#ifndef KOISHIDB_SRC_INCLUDE_MEMORY_WRITE_BATCH_H
#define KOISHIDB_SRC_INCLUDE_MEMORY_WRITE_BATCH_H

#include <string>
#include <vector>

#include "common/common.h"
#include "memory/memtable.h"
#include "util/util.h"

namespace koishidb {
// WriteBatch format

// kWriteBatchHeader = 12
// sequence(8) | count(4)
// -> KeyType varint (but now it's char)
// -> user_key varint value
// user_key = key | value
class WriteBatch {
 public:
  WriteBatch();

  WriteBatch(const WriteBatch &that) = default;

  WriteBatch &operator=(const WriteBatch &that) = default;

  ~WriteBatch() = default;

  void Put(const Slice &key, const Slice &value);

  void Delete(const Slice &key);

  // iterate the rep_ to insert into the skiplist
  void InsertAll(Memtable *memtable);

  void Append(const WriteBatch &that);

#ifdef DEBUG
  std::vector<std::pair<std::string, std::string>> Iterate();
#endif

  size_t EstimatedSize();

  void Clear();

 private:
  friend class WriteBatchInternal;

  std::string rep_;
};

// provide static member functions to hide the interface that we don't want to
// expose to outer
class WriteBatchInternal {
 public:
  static int Count(const WriteBatch *w);

  static void SetCount(WriteBatch *w, int count);

  static SequenceNumber Sequence(WriteBatch *w);

  static void SetSequence(WriteBatch *w, SequenceNumber seq);

  static void Append(const WriteBatch *src, WriteBatch *dst);

  static void InsertIntoMemtable(WriteBatch *src, Memtable *memtable);

  static size_t Bytes(WriteBatch *w) { return w->rep_.size(); };
};

#ifdef DEBUG
// used only for debug
inline std::vector<std::pair<std::string, std::string>> WriteBatch::Iterate() {
  int count = WriteBatchInternal::Count(this);
  SequenceNumber number = WriteBatchInternal::Sequence(this);
  Slice entries(rep_);
  // skip the header (12 bytes)
  entries.Advance(kWriteBatchHeader);
  std::vector<std::pair<std::string, std::string>> result;
  for (int i = 0; i < count; i++) {
    char key_type = entries.data()[0];
    entries.Advance(1);
    switch (static_cast<KeyType>(key_type)) {
      case KeyType::kTypeValue: {
        std::string user_key;
        GetLengthPrefixedSlice(&user_key, &entries);
        std::string value;
        GetLengthPrefixedSlice(&value, &entries);
        result.push_back({user_key, value});
        break;
      }
      case KeyType::kTypeDeletion: {
        std::string user_key;
        GetLengthPrefixedSlice(&user_key, &entries);
        result.push_back({user_key, ""});
        break;
      }
      default: {
        // Panic！
        return {};
      }
    }
  }
  return result;
}
#endif

};  // namespace koishidb

#endif