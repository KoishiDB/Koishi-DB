#include "disk/table_builder.h"
#include "disk/block_builder.h"
#include "disk/format.h"
#include "disk/writable_file.h"
#include "common/option.h"
#include "common/common.h"
#include "util/comparator.h"
#include "disk/filterblock_builder.h"


namespace koishidb {
  // TODO
  struct TableBuilder::Rep {
    Rep(const Option* opt, WritableFile* file): opt(opt), file(file), data_block(opt), index_block(opt), filter_block(),
    offset(static_cast<uint64_t>(0)), num_entries(static_cast<uint64_t>(0)), closed(false), block_handle(), pending_index_block(false)
    ,last_key("") {}

    // can't change the option in current implementation
    const Option* opt; // contains the comparator
    WritableFile* file;
    BlockBuilder data_block;
    BlockBuilder index_block;
    FilterBlockBuilder filter_block;
    uint64_t offset;
    uint64_t num_entries;
    bool closed; // when call the finish or abandon, closed = true
    BlockHandle block_handle; // used to insert into the index block;
    bool pending_index_block; // flag to mark add one index entry;
    std::string last_key; // to compare
    Status status; // status
  };
  TableBuilder::TableBuilder(const Option* opt, WritableFile* writableFile): rep_(new Rep(opt, writableFile)) {}

  TableBuilder::~TableBuilder() {
    delete rep_;
  }

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

    r->filter_block.AddKey(key);

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
      r->filter_block.StartBlock(r->offset);
    }
  }

  void TableBuilder::WriteBlock(BlockBuilder *block_builder, BlockHandle *block_handle) {
    Rep* r = rep_;
    Slice block_content = block_builder->Finish();
    // TODO
    r->status = r->file->Append(block_content); // write to buffer
    if (r->status.ok()) {
      block_handle->set_offset(r->offset);
      block_handle->set_size(block_content.size());
      r->offset += block_content.size();
    }
  }

  // finish part to do
  Status TableBuilder::Finish() {
    Rep* r = rep_;
    assert(!r->closed);
    r->closed = true;

    Flush(); // flush the left data block.
    if (!ok()) {
      return status();
    }
    // currently we have write the all the data blocks and build up a newly index block
    // what we need to do is to write them to the table file.
    // 1.Write Filter Blocks->
    // 2.Write MetaIndex Block
    // 3.Write IndexBlock
    // 4.Write Footer

    BlockHandle index_block_handle, filter_block_handle;

    Slice filter_block_contents = r->filter_block.Finish();
    r->status = r->file->Append(filter_block_contents);
    if (!r->status.ok()) {
      return r->status;
    }
    filter_block_handle.set_offset(r->offset);
    filter_block_handle.set_size(filter_block_contents.size());
    r->offset += filter_block_contents.size();

    // write index_block
    if (r->pending_index_block) {
      std::string block_handle;
      r->block_handle.EncodeTo(&block_handle);
      r->index_block.Add(Slice(r->last_key), Slice(block_handle));
      r->pending_index_block = false;
    }
    WriteBlock(&r->index_block, &index_block_handle);

    if (!ok()) {
      return status();
    }

    // footer format (fixed size)
    // filter_block_handle, index_block_handle, paddings(40 - sizeof(filter_block_handle) - sizeof(index_block_handler))
    Footer footer;
    footer.set_index(index_block_handle);
    footer.set_filter(filter_block_handle);

    std::string footer_encoded;
    footer.EncodeTo(&footer_encoded);
    r->status = r->file->Append(footer_encoded.data());
    if (!ok()) {
      return status();
    }
    r->offset += footer_encoded.size(); // The same as the kFixedFooterSize;
    r->file->Flush();
    return status();
  }

  void TableBuilder::Abandon() {
      assert(!rep_->closed);
      rep_->closed = true;
  }


  uint64_t TableBuilder::NumEntries() const { return rep_->num_entries; }

  uint64_t TableBuilder::FileSize() const { return rep_->offset; }

  Status TableBuilder::status() const { return rep_->status; }
};