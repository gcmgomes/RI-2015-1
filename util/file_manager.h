#ifndef _RI_2015_1_UTIL_FILE_MANAGER_H_
#define _RI_2015_1_UTIL_FILE_MANAGER_H_

#include <iostream>
#include <fstream>
#include <priority_queue>
#include <vector>
#include <string>
#include "tuple.h"

namespace util {

class FileManager {
  public:
    FileManager(unsigned file_count, std::string file_prefix, std::string output_file);
    
    // Get next tuple from the file |file_prefix_| + |tuple_file_id| at the position
    // |tuple_id|. Returns nullptr if EOF has already been reached, otherwise the caller
    // takes ownership of the returned object.
    Tuple* GetNextTuple(unsigned tuple_file_id, unsigned tuple_id);
    
    // Performs output operations for the necessary Tuple fields to |output_file_|.
    void OutputTuple(Tuple* tuple);
    
    // Initializes |heap| with exactly 1 Tuple from each one of the |file_count_| files.
    void InitializeHeap(std::priority_queue<Tuple, vector<Tuple>, &Tuple::LessThen>* heap);
    
  private:
    unsigned file_count_;
    std::string file_prefix_;
    std::ifstream input_file_;
    std::ofstream output_file_;
};

} //namespace util 

#endif
