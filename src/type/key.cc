#include "type/key.h"

#include <string>

namespace koishidb {

InternalKey::InternalKey(const Slice& user_key, SequenceNumber sequence,
                         KeyType type) {
  rep_ = user_key.ToString();
  uint64_t tag = sequence << 8 | static_cast<char>(type);
  rep_ += std::to_string(tag);
}

InternalKey::InternalKey(const std::string& rep) : rep_(rep) {}

std::string InternalKey::ToString() const { return rep_; }

Slice InternalKey::ToSlice() const { return rep_; }

void InternalKey::Clear() { rep_.clear(); }

bool InternalKey::Valid() const { return !rep_.empty(); }

InternalKey TransToInternalKey(const Slice& key) {
  return InternalKey(key.ToString());
}

std::string InternalKeyComparator::Name() const {
  return "InternalKeyComparator";
}

int InternalKeyComparator::Compare(const Slice& a, const Slice& b) const {
  Slice user_key1, user_key2;
  SequenceNumber seq1, seq2;
  ExtractUserKey(a, &user_key1, &seq1);
  ExtractUserKey(b, &user_key2, &seq2);
  int r = user_key1.Compare(user_key2);
  if (r == 0) {
    if (seq1 < seq2) {
      r = +1;
    } else if (seq1 > seq2) {
      r = -1;
    }
  }
  return r;
}

int InternalKeyComparator::Compare(const InternalKey& a,
                                   const InternalKey& b) const {
  return Compare(a.ToSlice(), b.ToSlice());
}

}  // namespace koishidb