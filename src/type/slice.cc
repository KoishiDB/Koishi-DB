#include "type/slice.h"
#include "logger.h"

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
    bool Slice::operator==(const Slice &that) const {
        if (that.size() != size_) {
            return false;
        }
        int ret =  memcmp(data_, that.data(), size_);
        return ret == 0;
    }

    bool Slice::operator!=(const Slice &that) const {
        return !(*this == that);
    }

    int Slice::Compare(const Slice& that) const {
        const size_t min_len = (size_ < that.size_) ? size_ : that.size_;
        int r = memcmp(data_, that.data_, min_len);
        if (r == 0) {
            if (size_ < that.size_)
                r = -1;
            else if (size_ > that.size_)
                r = +1;
        }
        return r;

    }

    std::string Slice::ToString() {
        return std::string(data_, size_);
    }

    void Slice::Advance(size_t n) {
        assert(n < size_);
        data_ += n;
        size_ -= n;
    }

    void Slice::Clear() {
        data_ = "";
        size_ = 0;
    }
};

