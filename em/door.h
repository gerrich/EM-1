#pragma once

#include "file.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>

using std::max;
using std::min;

#include <boost/shared_array.hpp>

namespace em {

template <typename TData>
class door {
 public:
  static const size_t npos = -1;
  static const size_t block_size = 1024*1024;
  static const size_t round_block_size = (block_size / sizeof(TData)) * sizeof(TData);

  class read_iterator: public std::iterator<std::forward_iterator_tag, TData, ptrdiff_t, TData*, const TData&> {
   public:
    explicit read_iterator(const door &door__)
      : offset_(npos) 
      , door_(door__) {
    }
  
    read_iterator(const door &door__, size_t offset)
      : offset_(offset) 
      , door_(door__)
      , buffer_(new TData[round_block_size]) {
    }

    read_iterator& operator ++() {
      ++offset_;
      if (offset_ % round_block_size == 0) { // need to read data from file
        door_.file_.seek(offset_ + door_.offset_);
        door_.file_.read((void*)(buffer_.get()), round_block_size);
      }
      return *this;
    }
    
    const TData& operator*() const {
      return buffer_[offset_ % round_block_size];
    }

    bool operator == (const read_iterator& rhs) const {
      return (
          (offset_ == door_.npos || offset_ == door_.offset_ + door_.size_) &&
          (rhs.offset_ == rhs.door_.npos || rhs.offset_ == rhs.door_.offset_ + rhs.door_.size_)) ||
          offset_ == rhs.offset_;
    }
    bool operator != (const read_iterator& rhs) const {
      return !operator==(rhs);
    }

   private:
    size_t offset_;
    const door &door_;
    boost::shared_array<TData> buffer_;
  };

  class write_iterator : public std::iterator<std::forward_iterator_tag, TData> {
   public:
    explicit write_iterator(door &door__)
      : offset_(npos) 
      , door_(door__) {
    }
  
    write_iterator(door &door__, size_t offset)
      : offset_(offset) 
      , door_(door__)
      , buffer_(new TData[round_block_size]) {
    }

    write_iterator& operator ++() {
      ++offset_;
      if (offset_ % round_block_size == 0) { // need to write data from file
        door_.file_.seek(offset_ - round_block_size + door_.offset_);
        door_.file_.write((void*)(buffer_.get()), round_block_size);
      }
      return *this;
    }

    ~write_iterator() {
      size_t round_offset = offset_ - (offset_ % round_block_size);
      size_t size_to_write = min(door_.size_ - round_offset, size_t(round_block_size));
      if (size_to_write > 0) {
        door_.file_.seek(round_offset + door_.offset_);
        door_.file_.write((void*)(buffer_.get()), size_to_write);
      }
    }

    TData& operator*() {
      return buffer_[offset_ % round_block_size];
    }

   private:
    size_t offset_;
    door &door_;
    boost::shared_array<TData> buffer_;
  };

  door(TFile& file, size_t offset, size_t size)
      : file_(file)
      , offset_(offset)
      , size_(size) {
  }
  
  const read_iterator begin() const {
    return read_iterator(*this, 0);
  }
  write_iterator begin() {
    return write_iterator(*this, 0);
  }
  
  const read_iterator end() const {
    return read_iterator(*this);
  }
  write_iterator end() {
    return write_iterator(*this);
  }

 public:
  TFile &file_;
  size_t offset_;
  size_t size_;
};



} // namespace em {
