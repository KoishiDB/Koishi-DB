#include "disk/sstable_iterator.h"
#include "disk/sstable.h"
#include "disk/format.h"
#include "disk/block.h"
#include "disk/block_iterator.h"

namespace koishidb {

// easy version of the table level iterator
void SSTableIterator::SeekToFirst() {
  index_iter_->SeekToFirst();
  UpdateDataIterator();
}

void SSTableIterator::Next() {
    // MayBe there is a empty block;
    assert(data_iter_ != nullptr);
    while (true) {
      data_iter_->Next();
      if (data_iter_->Valid()) {
        break;
      }
      index_iter_->Next();
      if (index_iter_->Valid()) {
        UpdateDataIterator();
      } else {
        break;
      }
    }
}

void SSTableIterator::Prev() {
  // empty op now;
}

void SSTableIterator::SeekToLast() {
  // every time index_iter ->
  index_iter_->SeekToLast();
}

Slice SSTableIterator::Value() const {
  return data_iter_->Value();
}

Slice SSTableIterator::Key() const {
  return data_iter_->Key();
}

bool SSTableIterator::Valid() const {
  return data_iter_->Valid();
}
bool SSTableIterator::Seek(const Slice &target) {
  index_iter_->Seek(target);
  Slice block_handle = index_iter_->Value();
  if (std::string(block_handle.data(), block_handle.size()) != data_block_handle_) {
    UpdateDataIterator();
  }

  assert(data_iter_ != nullptr);
  return data_iter_->Seek(target);
}

Iterator* SSTableIterator::BlockReader(SSTable *table, const Slice &index_value) {
    BlockHandle block_handle;
    Slice input = index_value;

    auto s = block_handle.DecodeFrom(&input);
    if (!s.ok()) {
      return nullptr;
    }
    auto block_content = ReadBlock(table->random_access_file(), block_handle);
    if (!block_content.has_value()) {
      return nullptr;
    }
    Block* block = new Block(block_content.value());

    return new BlockIterator(block);
}

void SSTableIterator::UpdateDataIterator() {
    if (!index_iter_->Valid()) {
      data_iter_ = nullptr;
      return;
    }
    Slice block_handle = index_iter_->Value();
    if (data_iter_ != nullptr) {
      delete data_iter_; // new create
    }
    data_iter_ = BlockReader(table_, block_handle);
    data_block_handle_ = std::string(block_handle.data(), block_handle.size());
    // We need to use block_handle to get the data_iter
}

SSTableIterator::~SSTableIterator() {
  if (data_iter_ != nullptr) {
    delete data_iter_;
  }
}

SSTableIterator::SSTableIterator(Block *indexBlock) {
  index_iter_ = new BlockIterator(indexBlock);
}

};