#include "disk/table_iterator.h"
#include "disk/table.h"
#include "disk/format.h"
#include "disk/block.h"
#include "disk/block_iterator.h"

namespace koishidb {

// easy version of the table level iterator
void TableIterator::SeekToFirst() {
  index_iter_->SeekToFirst();
  UpdateDataIterator();
}

void TableIterator::Next() {
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

void TableIterator::Prev() {
  // empty op now;
}

void TableIterator::SeekToLast() {
  // every time index_iter ->
  index_iter_->SeekToLast();
}

Slice TableIterator::Value() const {
  return data_iter_->Value();
}

Slice TableIterator::Key() const {
  return data_iter_->Key();
}

bool TableIterator::Valid() const {
  return data_iter_->Valid();
}
bool TableIterator::Seek(const Slice &target) {
  index_iter_->Seek(target);
  Slice block_handle = index_iter_->Value();
  if (std::string(block_handle.data(), block_handle.size()) != data_block_handle_) {
    UpdateDataIterator();
  }

  assert(data_iter_ != nullptr);
  return data_iter_->Seek(target);
}

Iterator* TableIterator::BlockReader(SSTable *table, const Slice &index_value) {
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

void TableIterator::UpdateDataIterator() {
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

TableIterator::~TableIterator() {
  if (data_iter_ != nullptr) {
    delete data_iter_;
  }
}

TableIterator::TableIterator(Block *indexBlock) {
  index_iter_ = new BlockIterator(indexBlock);
}

};