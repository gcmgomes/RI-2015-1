#ifndef _RI_2015_1_EXTERNAL_SORTER_EXTERNAL_SORTER_H_
#define _RI_2015_1_EXTERNAL_SORTER_EXTERNAL_SORTER_H_

#include <iostream>
#include <priority_queue>
#include <vector>
#include "../util/tuple.h"

class ExternalSorter {
 public:
  ExternalSorter(unsigned max_tuple_buffer_size, unsigned max_heap_size);

  ~ExternalSorter();
 private:
  // Maximum number of tuples to be stored in memory before being flushed.
  unsigned max_tuple_buffer_size_;
  // Array of tuples to be flushed when max_tuple_buffer_size_ is reached.
  std::vector<Touple> tuple_buffer_;

  // Maximum number of tuples contained that sorting_heap_ can hold.
  unsigned max_heap_size_;
  // Heap of Tuples that will be sorted.
  std::priority_queue<Tuple, std::vector<Tuple>, &Tuple::LessThen> heap_;
};

#endif
