#ifndef _RI_2015_1_EXTERNAL_SORTER_EXTERNAL_SORTER_H_
#define _RI_2015_1_EXTERNAL_SORTER_EXTERNAL_SORTER_H_

#include <iostream>
#include <priority_queue>
#include <vector>
#include "../util/tuple.h"
#include "../util/file_manager.h"

namespace sorting {

class ExternalSorter {
 public:
 // Takes ownership of |file_manager|.
  ExternalSorter(util::FileManager* file_manager);

  ~ExternalSorter();
  
  void MultiwayMerge();
  
 private:
  // Manager for file operations.
  util::FileManager* file_manager_;
  
  // Heap of Tuples that will be sorted.
  std::priority_queue<util::Tuple, std::vector<util::Tuple>, &util::Tuple::LessThan> heap_;
};

} // namespace sorting

#endif
