#pragma once

#include <cstdio>
#include <cstdlib>

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <algorithm>
#include <utility>

using std::max;
using std::min;
using std::make_pair;

namespace em {

class TFile {
 public:
  static const size_t npos = -1;

  TFile(size_t size = npos)
      : size_(0)
  {
    char tmpl[] = "file_xxxxxx";
    file_ = ::mkstemp(tmpl);
    if (size != npos) {
      ftruncate(file_, size);
      size_ = size;
    }
  }

  TFile(const char* name, size_t size = npos)
      :size_(0) {
    struct stat filestatus;
    stat(name, &filestatus);
    size_ = filestatus.st_size;

    file_ = ::open(name, O_RDWR|O_CREAT);
    if (size != npos) {
      ftruncate(file_, size);
      size_ = size;
    }
  }

  ~TFile() {
    ::close(file_);
  }

  size_t read(void *buf, size_t size) const {
    return ::read(file_, buf, size);
  }

  size_t write(void *buf, size_t size) {
    size_t bytes_written = ::write(file_, buf, size);
    size_ = max(size_, (size_t)::lseek(file_, 0, SEEK_CUR));
    return bytes_written;
  }

  size_t seek(size_t offset) {
    return ::lseek(file_, offset, SEEK_SET);
  }

  size_t size() const {
    return size_;
  }

 private:
  int file_;
  size_t size_;
};

} //namespace em {
