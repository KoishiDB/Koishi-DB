#include "type/slice.h"

namespace koishidb {
    const char *Slice::data() const {
        return data_;
    }
    size_t Slice::size() const {
        return size_;
    }
    bool Slice::empty() const {
        return size_ == 0;
    }
    Slice::Slice() : data_(""), size_(0) {}
    Slice::Slice(const char* d) : data_(d), size_(strlen(d)) {}
    Slice::Slice(const char *d, size_t size) : data_(d), size_(size) {}
    Slice::Slice(std::string &s) : data_(s.data()), size_(s.size()) {}

    char Slice::operator[](size_t n) const {
        assert(n < size_);
        return data_[n];
    }


    int Compare(const Slice& a, const Slice& b) {
      return strcmp(a.data(), b.data());
    }
};
