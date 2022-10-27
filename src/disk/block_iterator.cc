#include "disk/block_iterator.h"
#include "disk/block.h"
#include "type/key.h"
#include "util/util.h"

#include <iostream>
namespace koishidb {
    // TODO, replace the cmp_ with internal key comparator
    BlockIterator::BlockIterator(Block *block) : data_(block->data), size_(block->size), cmp_(nullptr){
        SeekToFirst();
    }
    BlockIterator::~BlockIterator() {
        delete cmp_;
    }

    void BlockIterator::SeekToFirst() {
        offset_ = 0;
        key_.clear();
        value_ = Slice(data_, 0); // empty
    }

    void BlockIterator::Next() {
        //  next entry offset

        offset_ = value_.data() + value_.size() - data_;
        if (!this->Valid()) {
            return;
        }
        Slice rest_entries = Slice(data_ + offset_, size_ - offset_);
        uint32_t internal_key_len;
        GetVarint32(&rest_entries, &internal_key_len);
        key_ = std::string(rest_entries.data(), internal_key_len);
        rest_entries.Advance(internal_key_len);

        uint32_t  value_len;
        GetVarint32(&rest_entries, &value_len);
        value_ = Slice(rest_entries.data(), value_len);
    }

    // iff return true, the Iterator.Key() == target
    //
    bool BlockIterator::Seek(const Slice &target) {
        SeekToFirst();
        // linear
        while (cmp_->Compare(Key(), target) == -1) {
            Next();
        }

        if (cmp_->Compare(Key(), target) == 0) {
            return true;
        } else {
            return false;
        }
    }

    Slice BlockIterator::Key() const {
       return key_;
    }

    Slice BlockIterator::Value() const {
        return value_;
    }

    bool BlockIterator::Valid() const {
        return offset_ < size_;
    }

    void BlockIterator::Prev() {
        // empty
    }

    void BlockIterator::SeekToLast() {

    }
};