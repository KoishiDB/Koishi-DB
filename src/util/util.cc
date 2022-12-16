#include "util/util.h"

#include <fcntl.h>
#include <unistd.h>

#include "util/encode.h"
namespace koishidb {

void ExtractInternalKey(Slice memtable_key, Slice* internal_key) {
  // In fact, metable_key is the entry that inserted into the skiplist
  uint32_t internal_key_len;
  GetVarint32(&memtable_key, &internal_key_len);
  GetFixedBytes(&memtable_key, internal_key, internal_key_len);
}

void ExtractUserKey(const Slice& internal_key, Slice* user_key,
                    SequenceNumber* number) {
  *user_key = Slice(internal_key.data(), internal_key.size() - 8);
  *number = (*reinterpret_cast<const SequenceNumber*>(internal_key.data() +
                                                      user_key->size()) >>
             8);
}

// Create an InternalKey
// When we create the internal key, we should be careful when dump it into the
// disk we should free the memory of it
Slice CreateInternalKey(const Slice& user_key, SequenceNumber snapshot,
                        KeyType type) {
  size_t usize = user_key.size();
  size_t needed = usize + 8;
  SequenceNumber tag = snapshot << 8 | static_cast<char>(type);
  // need to check here, user_key should not be too big
  char* dst = new char[needed];
  memcpy(dst, user_key.data(), usize);
  *reinterpret_cast<SequenceNumber*>(dst + usize) = tag;

  return Slice(dst, needed);
}

// Create a memtable key
// When we create the memtable key, we should be careful when dump it into the
// disk we should free the memory of it
Slice CreateMemtableKey(const Slice& user_key, SequenceNumber snapshot,
                        const Slice& value, KeyType key_type) {
  size_t usize = user_key.size();
  size_t vsize = value.size();
  size_t needed = usize + 8 + vsize + 10;  // A conservative size
  char* dst = new char[needed];
  size_t advance =
      EncodeVarint32(usize + 8, dst);  // usize + 8 == internal_key length;
  memcpy(dst + advance, user_key.data(), usize);
  uint64_t tag = snapshot << 8 | static_cast<char>(key_type);
  const char* ptr = reinterpret_cast<const char*>(&tag);
  for (int i = 0; i < 8; i++) {
    // dst + advance + usize
    dst[advance + usize + i] = ptr[i];
  }
  char* ptr2 = dst + advance + usize + 8;
  size_t advance2 = EncodeVarint32(vsize, ptr2);
  memcpy(ptr2 + advance2, value.data(), vsize);
  size_t size = usize + 8 + advance + advance2 + vsize;
  return Slice(dst, size);
}

// TODO, we need to handle the wrong format memtable_key to avoid the crush.
void ExtractValueFromMemtable(Slice memtable_key, std::string* result) {
  uint32_t internal_key_len;
  GetVarint32(&memtable_key, &internal_key_len);
  memtable_key.Advance(internal_key_len);
  uint32_t value_len;
  GetVarint32(&memtable_key, &value_len);
  result->assign(memtable_key.data(), value_len);
}

bool Remove(const std::string& file_name) {
  int result = ::remove(file_name.data());

  if (result == -1) {
    return false;
  }
  return true;
}

}  // namespace koishidb