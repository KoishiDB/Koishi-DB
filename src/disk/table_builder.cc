#include "disk/table_builder.h"
#include "disk/block_builder.h"
#include "disk/format.h"
#include "common/option.h"
#include "common/common.h"
#include "util/comparator.h"


namespace koishidb {
  // TODO
  class FilterBlockBuilder;
  struct TableBuilder::Rep {
    Rep(const Option* opt, WritableFile* file): opt(opt), file(file), data_block(opt), index_block(opt), meta_block(nullptr)
    ,offset(static_cast<uint64_t>(0)), num_entries(static_cast<uint64_t>(0)), closed(false), block_handle(), pending_index_block(false)
    ,last_key("") {}

    // can't change the option in current implemtation.
    const Option* opt; // contains the comparator
    WritableFile* file;
    BlockBuilder data_block;
    BlockBuilder index_block;
    FilterBlockBuilder* meta_block; // TODO, currently no uesd
    uint64_t offset;
    uint64_t num_entries;
    bool closed; // when call the finish or abandon, closed = true
    BlockHandle block_handle; // used to insert into the index block;
    bool pending_index_block; // flag to mark add one index entry;
    std::string last_key; // to compare
    Status status; // status
  };
  TableBuilder::TableBuilder(const Option* opt, WritableFile* writableFile): rep_(new Rep(opt, writableFile)) {}

  void TableBuilder::Add(const Slice &key, const Slice &value) {
    Rep* r = rep_;
    assert(!r->closed); // should not closed

    assert(r->last_key.empty() || r->opt->cmp->Compare(key, r->last_key.c_str()) == 1);

    if (r->pending_index_block) {
      assert(r->data_block.Empty());

      // Add to index block (last_key, BlockHandle)
      std::string block_handle;
      r->block_handle.EncodeTo(&block_handle);
      r->index_block.Add(Slice(r->last_key), Slice(block_handle));
      r->pending_index_block = false;
    }
    // TODO filter block write

    r->data_block.Add(key, value);
    r->last_key.assign(key.data(), key.size()); // use std::string.assign(), can hold string or c_str
    r->num_entries++;

    if (r->data_block.EstimatedSize() >= kBlockSize) {
      // write to writableFile
      Flush();
    }
  }
  // TODO, need to add some error handler
  void TableBuilder::Flush() {
    Rep* r = rep_;
    assert(!r->closed);
    if (r->data_block.Empty()) {
      return;
    }
    WriteBlock(&r->data_block, &r->block_handle);
    // clear the data_block builder
    if (ok()) {
      r->pending_index_block = true;
      r->status = r->file->Flush();
    }

    // TODO, write the meta block.
  }

  void TableBuilder::WriteBlock(BlockBuilder *block_builder, BlockHandle *block_handle) {
    Rep* r = rep_;
    Slice block_content = block_builder->Finish();
    // TODO
    r->status = r->file->Apppend(block_content); // write to buffer
    if (r->status.ok()) {
      block_handle->set_offset(r->offset);
      block_handle->set_size(block_content.size());
      r->offset += block_content.size(); //
    }
  }

  // finish part to do
  Status TableBuilder::Finish() {
    Rep* r = rep_;
    assert(!r->closed);

    // TODO
    // 1.Write Meta Blocks->
    // 2.Write MetaIndex Block
    // 3.Write IndexBlock
    // 4.Write Footer
  }

  void TableBuilder::Abandon() {
      assert(!rep_->closed);
      rep_->closed = true;
  }


  uint64_t TableBuilder::NumEntries() const { return rep_->num_entries; }

  uint64_t TableBuilder::FileSize() const { return rep_->offset; }

  Status TableBuilder::status() const { return rep_->status; }
};