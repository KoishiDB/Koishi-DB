#include "disk/block_builder.h"
#include "util/encode.h"
#include "util/comparator.h"
#include "common/common.h"
#include "common/option.h"


namespace koishidb {
  // Key -> Internal_key ->
  // Value -> might be empty
  void BlockBuilder::Add(const Slice &key, const Slice &value) {
      // Data format -> varint: Internal_key.size(), value.size()
      // key, value;
      Slice last_key = Slice(last_key_);
      assert(last_key.empty() || opt_->cmp->operator()(key, last_key) == 1);
      PutVarint32(key.size(), &rep_);
      PutVarint32(value.size(), &rep_);
      rep_.append(key.data(), key.size());
      rep_.append(value.data(), value.size());
  }

  size_t BlockBuilder::EstimatedSize() {
      return rep_.size();
  }

  void BlockBuilder::Reset() {
      is_finished_ = false;
      rep_.clear();
      last_key_.clear();
  }

  Slice BlockBuilder::Finish() {
      // how to finish a block builder ?
      is_finished_ = true;
      return Slice(rep_);
  }
};