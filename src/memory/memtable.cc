#include "memory/memtable.h"
#include "common/common.h"
#include "util/comparator.h"
#include "type/key.h"

namespace koishidb {

// We don't need the memtable Iterator to seek for key, we just need it to
// iterate the memtable and dump it to disk
class MemtableIterator : public Iterator {
 public:
  ~MemtableIterator() = default;

  MemtableIterator(Memtable::Table* table) : iter_(table) {}
  bool Valid() const override { return iter_.Valid(); }
  void Next() override { iter_++; }

  Slice Key() const override {
    Slice memtable_key = iter_.Key();
    Slice internal_key;
    ExtractInternalKey(memtable_key, &internal_key);
    return internal_key;
  }

  Slice Value() const override {
    Slice memtable_key = iter_.Key();
    uint32_t internal_key_len;
    GetVarint32(&memtable_key, &internal_key_len);
    memtable_key.Advance(internal_key_len);
    uint32_t value_len;
    GetVarint32(&memtable_key, &value_len);
    return Slice(memtable_key.data(), value_len);
  }

  void SeekToFirst() override { assert(iter_.Valid()); }

 private:
  // private name
  Memtable::Table::Iterator iter_;
  // this function is useless, we don't and should not use it
  bool Seek(const Slice& target) override { return false; }

  void SeekToLast() override {}

  void Prev() override {}
};

size_t Memtable::EstimatedSize() { return size_; }

bool Memtable::Get(const Slice& memtable_key, std::string* result) {
  Slice ret;
  bool flag = table_->FindFirstGreaterOrEqual(memtable_key, &ret);
  if (flag == false) {
    return false;
  }
  // ret -> first >=
  Slice internal_key1, internal_key2;
  Slice key(memtable_key.data(), memtable_key.size());
  ExtractInternalKey(ret, &internal_key1);
  ExtractInternalKey(key, &internal_key2);
  Slice user_key1, user_key2;
  SequenceNumber s1, s2;
  ExtractUserKey(internal_key1, &user_key1, &s1);
  ExtractUserKey(internal_key2, &user_key2, &s2);
  InternalKeyComparator cmp;
  if (user_key1.Compare(user_key2) != 0) {
    return false;
  }

  ExtractValueFromMemtable(ret, result);
  return true;
}

// We should always use the CreateMemtableKey to create memtable_key and insert
// it into the memtable, for we will call the delete method in BuildTable
// function
void Memtable::Insert(const Slice& memtable_key) {
  table_->Insert(memtable_key);
  size_ += memtable_key.size();
}

Iterator* Memtable::NewIterator() {
  Iterator* iter = new MemtableIterator(table_);
  return iter;
}
};  // namespace koishidb