#include "disk/sstable.h"
#include "disk/random_access_file.h"
#include "disk/format.h"
#include "disk/filterblock_reader.h"
#include "disk/block.h"
#include "disk/sstable_iterator.h"
#include "common/option.h"
#include "common/common.h"
#include "logger.h"
namespace koishidb {

struct SSTable::Rep {
  ~Rep() {
    delete filter_block_reader;
    delete[] filter_data;
    delete index_block;
  }
  const Option* option;
  // something that should be a ptr;
  RandomAccessFile* file;
  Status status;
  FilterBlockReader* filter_block_reader;
  const char* filter_data;
  Block* index_block;
};


SSTable::~SSTable() {
    delete rep_;
}

std::optional<SSTable *> SSTable::Open(const Option *opt, RandomAccessFile *file, size_t file_size) {
    if (file_size < kFixedFooterSize) {
      LOG_ERROR("corrupted sstable file");
      return { };
    }
    char footer_space[kFixedFooterSize];
    Slice footer_content;

    Status s = file->Read(file_size - kFixedFooterSize, kFixedFooterSize, &footer_content, footer_space);
    if (!s.ok()) {
      return {};
    }
    Footer footer;
    s = footer.DecodeFrom(&footer_content);
    if (!s.ok()) {
      return {};
    }

    // Read Index Block
    auto index_block_content = ReadBlock(file, footer.index_handle());
    if (!index_block_content.has_value()) {
      return {};
    }

    Block* block = new Block(index_block_content.value().get());

    // optional 's behavior is like an iterator
    //set the rep_ now;

    // read meta;
    auto filter_block_content = ReadBlock(file, footer.filter_handle());
    if (!filter_block_content.has_value()) {
       delete block;
       return {};
    }

    Rep* rep = new SSTable::Rep;
    rep->index_block = block;
    rep->filter_data = filter_block_content.value()->data();
    rep->status = Status();
    rep->filter_block_reader = new FilterBlockReader(Slice(rep->filter_data));
    rep->file = file;
    rep->option = opt;
    SSTable* table = new SSTable(rep);
    table->file_ = file;
    return { table };
}


std::optional<std::unique_ptr<BlockContent>> ReadBlock(RandomAccessFile *file, const BlockHandle &block_handle) {
    // use smart pointer to automatically delete.
    std::unique_ptr<BlockContent> result(new BlockContent);

    char* block_content = new char[block_handle.size()];
    // use unique_ptr.get() to get the raw pointer and never acquire the ownership
    // can use unique_ptr.release() to release the ownership
    Status status = file->Read(block_handle.offset(), block_handle.size(), result.get(), block_content);
    if (!status.ok()) {
      delete[] block_content;
      return {};
    }
    if (result->size() != block_handle.size()) {
      delete[] block_content;
      LOG_ERROR("read block error");
      return {};
    }
    return { std::move(result) };
}

std::optional<std::string> SSTable::InternalGet(const koishidb::Slice &key) {
    // first find the bloom_filter;
    Rep *r = rep_;
    if (bloom_filter_.KeyMayMatch(key, r->filter_data) == false) {
        return {};
    }
    Iterator* iter = NewIterator();
    iter->SeekToFirst();
    bool flag = iter->Seek(key);
    if (!iter->Valid() || flag == false) {
        return {};
    }
    return { iter->Value().ToString() };

    delete iter;
}



Iterator* SSTable::NewIterator() const {
    return new SSTableIterator(rep_->index_block, rep_->file, rep_->option);
}
}; // namespace koishidb