#include "disk/table.h"
#include "disk/random_access_file.h"
#include "disk/format.h"
#include "disk/filterblock_reader.h"
#include "disk/block.h"
#include "common/option.h"
#include "common/common.h"
#include "logger.h"
namespace koishidb {

struct Table::Rep {
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

  BlockHandle filter_block_handler;
  Block* index_block;
};



std::optional<Table *> Table::Open(const Option *opt, RandomAccessFile *file, size_t file_size) {
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

    Block* block = new Block(index_block_content.value());
    // optional 's behavior is like an iterator
    //set the rep_ now;
    Rep* rep = new Table::Rep;
    rep->index_block = block;
    rep->option = opt;
    rep->status = Status();

    // next read meta

    return {};
}

std::optional<BlockContent*> ReadBlock(RandomAccessFile *file, const BlockHandle &block_handle) {
    BlockContent* result;

    char* block_content = new char[block_handle.size()];
    Status status = file->Read(block_handle.offset(), block_handle.size(), result, block_content);
    if (!status.ok()) {
      delete[] block_content;
      return {};
    }
    if (result->size() != block_handle.size()) {
      delete[] block_content;
      LOG_ERROR("read block error");
      return {};
    }

    return { result };

}

}; // namespace koishidb