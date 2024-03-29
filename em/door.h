#pragma once

#include "defs.h"
#include "file.h"
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>

using std::max;
using std::min;

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

namespace em {

template <typename TData>
class door {
 public:
  static const size_t round_block_size = block_size - block_size % sizeof(TData);
  static const size_t npos = -1;

  class read_iterator: public std::iterator<std::forward_iterator_tag, TData, ptrdiff_t, TData*, const TData&> {
   public:
    explicit read_iterator(const door &door__)
      : offset_(npos) 
      , door_(door__) {
    }
  
    read_iterator(const door &door__, size_t offset)
      : offset_(offset) 
      , door_(door__)
      , buffer_(new TData[round_block_size / sizeof(TData)]) {
        door_.file_.seek(offset_ + door_.offset_);
        door_.file_.read((void*)(buffer_.get()), round_block_size);
    }

    read_iterator& operator ++() {
      offset_ += sizeof(TData);
      if (offset_ % round_block_size == 0) { // need to read data from file
        door_.file_.seek(offset_ + door_.offset_);
        door_.file_.read((void*)(buffer_.get()), round_block_size);
      }
      return *this;
    }
    
    const TData& operator*() const {
      return buffer_[(offset_ % round_block_size) / sizeof(TData)];
    }

    bool operator == (const read_iterator& rhs) const {
      return (
          (offset_ == door_.npos || offset_ == door_.size_) &&
          (rhs.offset_ == rhs.door_.npos || rhs.offset_ == rhs.door_.size_)) ||
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
      , buffer_(new TData[round_block_size / sizeof(TData)])
      , altered_flag_ptr_(new bool(false)) {
    }

    write_iterator& operator ++() {
      offset_ += sizeof(TData);
      if (offset_ % round_block_size == 0) { // need to write data from file
        //cerr << "time to write at: " << offset_ - round_block_size + door_.offset_ << std::endl;
        door_.file_.seek(offset_ - round_block_size + door_.offset_);
        door_.file_.write((void*)(buffer_.get()), round_block_size);
        door_.size_ = max(door_.size_, offset_); // make the door little bit wider
        *altered_flag_ptr_ = false;
      }
      return *this;
    }

    ~write_iterator() {
      if (*altered_flag_ptr_) {
        size_t round_offset = offset_ - (offset_ % round_block_size);
        size_t size_to_write = min(max(door_.size_, offset_) - round_offset, size_t(round_block_size));
        if (size_to_write > 0) {
          door_.file_.seek(round_offset + door_.offset_);
          door_.file_.write((void*)(buffer_.get()), size_to_write);
          door_.size_ = max(door_.size_, offset_); // enlarge the door size
        }
        *altered_flag_ptr_ = false;
      }
    }

    TData& operator*() {
      *altered_flag_ptr_ = true;
      return buffer_[(offset_ % round_block_size) / sizeof(TData)];
    }

   private:
    size_t offset_;
    door &door_;
    boost::shared_array<TData> buffer_;
    boost::shared_ptr<bool> altered_flag_ptr_;
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
