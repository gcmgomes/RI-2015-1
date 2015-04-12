#ifndef _RI_2015_1_UTIL_FILE_MANAGER_H_
#define _RI_2015_1_UTIL_FILE_MANAGER_H_

#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <vector>
#include <string>
#include "tuple.h"

namespace util {

class FileManager {
 public:
  // Input files are of the form |file_prefix|+[0, |file_count|).
  FileManager(unsigned file_count, std::string file_prefix,
              std::string output_file_path);

  ~FileManager();

  // Get next tuple based on |file_id|.
  // Returns NULL if EOF has already been reached, otherwise the caller
  // takes ownership of the returned object.
  std::unique_ptr<Tuple> GetNextTuple(unsigned file_id);

  // Writes |tuple| to |output_file_| without any special treatment.
  void WriteTuple(const Tuple* tuple);

  // Flushes |output_file_|.
  void Flush();

  // Close |output_file_|.
  void CloseOutput();

  // Initializes |heap| with exactly 1 Tuple from each one of the input files.
  void InitializeHeap(std::priority_queue<
      std::unique_ptr<util::Tuple>, std::vector<std::unique_ptr<util::Tuple> >,
      util::TupleCompare>* heap);

  // Divides the file pointed by |file_path| into the needed smaler, partially
  // sorted files.
  void Split(const std::string& file_path);

  // Converts |binary_input_file_path| into human-readable
  // |text_output_file_path|.
  static void ConvertBinToText(const std::string& binary_input_file_path,
                               const std::string& text_output_file_path);

 private:
  std::string file_prefix_;
  std::vector<std::unique_ptr<std::ifstream> > input_files_;
  std::ofstream output_file_;
};

}  // namespace util

#endif
