#pragma once

#include "file.h"
#include "door.h"
#include "vector.h"

#include <vector>
#include <algorithm>
#include <utility>

using std::max;
using std::min;
using std::make_pair;

namespace em {

template <typename TData, typename TComparator>
void sort(TFile &file, TComparator comparator) {
  const size_t block_size = 1024*1024*4;
  const size_t mem_size = block_size * 16;

  size_t round_mem_size = mem_size - (mem_size % sizeof(TData));
  typedef std::pair<size_t, size_t> coord;
  std::vector<coord> contents;
  cerr << "SORT windows:" << endl;
  for (size_t offset = 0; offset < file.size(); offset += round_mem_size) {
    size_t window_size = min(round_mem_size, file.size() - offset);
    cerr << "offset: " << offset << "\twind_size: " << window_size << endl; 
    contents.push_back(make_pair(offset, window_size));
    window<TData> w(file, offset, window_size);
    std::sort(w.begin(), w.end(), comparator);
    w.flush(file, offset);
  }
  cerr << "DONE:" << endl;
  cerr << "fsize: " << file.size() << endl;

  char tmp_name[] = "tmp-xxxxxx";
  TFile tmp_file(tmp_name, O_RDWR | O_CREAT | O_TRUNC, file.size());
  TFile* in_file_ptr = &file;
  TFile* out_file_ptr = &tmp_file;
  while(contents.size() > 1) {
    cerr << "!!!contents.size(): " << contents.size() << endl;
    size_t out_offset = 0;
    std::vector<coord> new_contents;
    for (size_t index = 0; index + 1 < contents.size(); index += 2) {
      const door<TData> door_first(*in_file_ptr, contents[index].first, contents[index].second);
      const door<TData> door_second(*in_file_ptr, contents[index + 1].first, contents[index + 1].second);
      size_t out_size = contents[index].second + contents[index + 1].second;
      door<TData> door_out(*out_file_ptr, out_offset, out_size);
      new_contents.push_back(make_pair(out_offset, out_size));
      std::merge(
          door_first.begin(), door_first.end(),
          door_second.begin(), door_second.end(),
          door_out.begin(), comparator);
      out_offset += out_size;
    }
    if (contents.size() % 2) {
      const door<TData> door_first(*in_file_ptr, contents.back().first, contents.back().second);
      door<TData> door_out(*out_file_ptr, out_offset, contents.back().second);
      new_contents.push_back(make_pair(out_offset, contents.back().second));
      std::copy(
          door_first.begin(), door_first.end(),
          door_out.begin());
    }
    std::swap(new_contents, contents);
    std::swap(in_file_ptr, out_file_ptr);
  }
  if (in_file_ptr != &file) { // swap if result left in tmp file
    const door<TData> door_first(*in_file_ptr, 0, file.size());
    door<TData> door_out(*out_file_ptr, 0, file.size());
    std::copy(
        door_first.begin(), door_first.end(),
        door_out.begin());
  }
}

template <typename TIterator, typename TComparator>
bool check_sorted (TIterator begin, TIterator end, TComparator comparator) {
  TIterator& it = begin;
  typename TIterator::value_type data;
  data = *it;
  ++it;
  for(size_t id = 0; it != end; ++it, ++id) {
    if (comparator(*it, data)) {
      cerr << "wrong order on id:" << id << endl;
      return false;
    }
    data = *it;
  }
  return true;
}

} // namespace em {
