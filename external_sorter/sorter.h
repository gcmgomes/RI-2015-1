#ifndef _RI_2015_1_SORTER_SORTER_H_
#define _RI_2015_1_SORTER_SORTER_H_

#include <iostream>
#include <queue>
#include <vector>
#include "../util/tuple.h"
#include "../util/file_manager.h"

namespace sorting {

class Sorter {
 public:
 // Takes ownership of |file_manager|.
  Sorter(util::FileManager* file_manager);

  ~Sorter();
  
  void ExternalMultiwayMerge();
  
 private:
  // Manager for file operations.
  util::FileManager* file_manager_;
  
  // Heap of Tuples that will be sorted.
  std::priority_queue<util::Tuple, std::vector<util::Tuple>, util::TupleCompare> heap_;
};

} // namespace sorting

#endif
