#ifndef KOISHIDB_SRC_INCLUDE_TYPE_SLICE_H
#define KOISHIDB_SRC_INCLUDE_TYPE_SLICE_H

#include <cstddef>
#include <cstring>
#include <string>
namespace koishidb {
    class Slice {
    public:
        Slice();
        Slice(const char* d);
        Slice(std::string& s);
        Slice(const char* d, size_t size);
        Slice(const Slice& that) = default; // copyable
        Slice& operator= (const Slice& that) = default;
        // getters
        const char* data() const;
        size_t size() const;
        bool empty() const;
        char operator[](size_t n) const;
        bool operator== (const Slice& that) const;
        bool operator!= (const Slice& that) const; // const
        bool operator< (const Slice& that) const;
        friend int Compare(const Slice& a, const Slice &b);
    private:
        const char* data_;
        size_t size_;
    };
};


#endif