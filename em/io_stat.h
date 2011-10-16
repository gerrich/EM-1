#pragma once

#include <iostream>
#include <cstddef>

namespace em {

using std::endl;

class TIOStat {
public:
  static void read(size_t size) {
    read_bytes_ += size;
    read_calls_ += 1;
  }
  static void write(size_t size) {
    write_bytes_ += size;
    write_calls_ += 1;
  }

  static std::ostream& print_stat(std::ostream &os) {
    os << "read_bytes: " << read_bytes_ << endl;
    os << "read_calls: " << read_calls_ << endl;
    os << "write_bytes: " << write_bytes_ << endl;
    os << "write_calls: " << write_calls_ << endl;
    return os;
  }
private:
  static size_t read_bytes_;
  static size_t read_calls_;
  static size_t write_bytes_;
  static size_t write_calls_;
};
size_t TIOStat::read_bytes_ = 0;
size_t TIOStat::read_calls_ = 0;
size_t TIOStat::write_bytes_ = 0;
size_t TIOStat::write_calls_ = 0;
} //namespace em {

