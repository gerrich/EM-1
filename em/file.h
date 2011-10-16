#pragma once

#include "io_stat.h"

#include <cstdio>
#include <cstdlib>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <algorithm>
#include <utility>

#include <iostream>
using std::cerr;
using std::endl;

using std::max;
using std::min;
using std::make_pair;

namespace em {

class TFile : public TIOStat {
 public:
  static const size_t npos = -1;

  TFile(size_t size = npos)
      : size_(0)
      , offset_(0)
  {
    char tmpl[] = "file_xxxxxx";
    file_ = ::mkstemp(tmpl);
    if (size != npos) {
      ftruncate(file_, size);
      size_ = size;
    }
  }

  TFile(const char* name, int flags, size_t size = npos)
      : size_(0)
      , offset_(0) {
    if (size == npos) {
      struct stat filestatus;
      stat(name, &filestatus);
      size_ = filestatus.st_size;
    }
    
    file_ = ::open(name, flags, S_IRUSR | S_IWUSR);
    
    if (size != npos) {
      ftruncate(file_, size);
      size_ = size;
    }
  }

  ~TFile() {
    ::close(file_);
  }

  size_t read(void *buf, size_t size) const {
    ::lseek(file_, offset_, SEEK_SET);
    TIOStat::read(size);
    return ::read(file_, buf, size);
  }

  size_t write(void *buf, size_t size) {
    //cerr << "old size: " << size_ << endl;
    //std::cerr << "write: " << size << " bytes at: " << offset_ << endl;
    ::lseek(file_, offset_, SEEK_SET);
    TIOStat::write(size);
    size_t bytes_written = ::write(file_, buf, size);
    offset_ += bytes_written;
    size_ = max(size_, offset_);
    //cerr << "new size: " << size_ << endl;
    return bytes_written;
  }

  size_t seek(size_t offset) {
    offset_ = offset;
    return offset_;
//    return ::lseek(file_, offset, SEEK_SET);
  }

  size_t size() const {
    return size_;
  }

 private:
  int file_;
  size_t size_;
  size_t offset_;
};

} //namespace em {
