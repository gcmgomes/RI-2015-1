#ifndef _RI_2015_1_UTIL_FILE_MANAGER_H_
#define _RI_2015_1_UTIL_FILE_MANAGER_H_

#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include <string>
#include "tuple.h"

namespace util {

class FileManager {
  public:
    // Input files are of the form |file_prefix|+[0, |file_count|).
    FileManager(unsigned file_count, std::string file_prefix, std::string output_file_path);
    
    ~FileManager();
    
    // Get next tuple based on |file_id|.
    // Returns NULL if EOF has already been reached, otherwise the caller
    // takes ownership of the returned object.
    Tuple* GetNextTuple(unsigned file_id);
    
    // Caches |tuple| to an auxiliary output buffer.
    void CacheTupleToBuffer(Tuple* tuple);

    //Writes |tuple| to |output_file_| without any special treatment.
    void UnformattedWrite(Tuple* tuple);
    
    // Outputs the auxiliar buffer to |output_file_|. 
    void Flush();

    // Close |output_file_|.
    void CloseOutput() {
      if (output_file_.is_open()) {
        output_file_.close();
      }
    }
    
    // Initializes |heap| with exactly 1 Tuple from each one of the input files.
    void InitializeHeap(std::priority_queue<Tuple>* heap);
    
    // Divides the file pointed by |file_path| into the needed smaler, partially sorted files.
    void Split(const std::string& file_path);
    
  private:
    std::string file_prefix_;
    std::vector<std::ifstream*> input_files_;
    std::vector<Tuple> output_buffer_;
    std::ofstream output_file_;
};

} //namespace util 

#endif
