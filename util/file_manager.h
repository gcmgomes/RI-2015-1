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
    FileManager(unsigned file_count, std::string file_prefix, std::string output_file_path);
    
    ~FileManager();
    
    // Get next tuple based on |file_id|.
    // Returns NULL if EOF has already been reached, otherwise the caller
    // takes ownership of the returned object.
    Tuple* GetNextTuple(unsigned file_id);
    
    // Performs output operations for the necessary Tuple fields to |output_file_|.
    void OutputTuple(Tuple* tuple);
    
    // Initializes |heap| with exactly 1 Tuple from each one of the input files.
    void InitializeHeap(std::priority_queue<Tuple, vector<Tuple>, &Tuple::LessThen>* heap);
    
  private:
    std::string file_prefix_;
    std::vector<std::ifstream*> input_files_;
    std::ofstream output_file_;
};

} //namespace util 

#endif
