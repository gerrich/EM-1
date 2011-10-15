#pragma once

#include "file.h"
#include <vector>

namespace em {

template <typename TData>
class window {
 public:
  typedef typename std::vector<TData>::const_iterator const_iterator;
  typedef typename std::vector<TData>::iterator iterator;

  window(TFile& file, size_t offset, size_t length) {
    feed(file,offset,length);
  }
  
  void feed(TFile& file, size_t offset, size_t length) {
    data_.resize(length / sizeof(TData));
    file.seek(offset);
    file.read(reinterpret_cast<void*>(&data_[0]), data_.size() * sizeof(TData));
  }

  const_iterator begin() const {
    return data_.begin();
  }
  iterator begin() {
    return data_.begin();
  }
  
  const_iterator end() const {
    return data_.end();
  }
  iterator end() {
    return data_.end();
  }
  
  void flush(TFile& file, size_t offset) const {
    file.seek(offset);
    file.write((void*)(&data_[0]), data_.size() * sizeof(TData));
  }

 public:
  std::vector<TData> data_;
};



} // namespace em {
