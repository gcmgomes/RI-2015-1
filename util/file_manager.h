#ifndef _RI_2015_1_UTIL_FILE_MANAGER_H_
#define _RI_2015_1_UTIL_FILE_MANAGER_H_

#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <vector>
#include <string>
#include "tuple.h"
#include "../components/indexer.h"

namespace util {

class FileManager {
 public:
  FileManager(unsigned file_count, std::string file_prefix,
              std::string output_file_path);

  ~FileManager();

  // Get next tuple based on |file_id|.
  // Returns NULL if EOF has already been reached, otherwise the caller
  // takes ownership of the returned object.
  std::unique_ptr<Tuple> GetNextTuple(unsigned file_id);

  // Writes |tuple| to |output_file_| without any special treatment.
  void WriteTuple(const Tuple* tuple);

  // Index |tuple|.
  void Index(const Tuple* tuple);

  // Finishes all index operations. MUST call when done indexing.
  void FinishIndex();

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
  void PartialEmplaceSort();

  // Converts |binary_input_file_path| into human-readable
  // |text_output_file_path|.
  static void ConvertBinToText(const std::string& binary_input_file_path,
                               const std::string& text_output_file_path);

  // Returns the amount of tuples present in |input_file_path|.
  static unsigned CountTuples(const std::string& input_file_path);

 private:
  // Properly initializes |input_file[|file_id|]|.
  void InputFileInitialization(unsigned file_id);

  // Returns true if |input_file[|file_id|]| should be closed.
  bool CloseForInput(unsigned file_id);

  unsigned input_tuples_;
  unsigned input_tuples_per_block_;
  std::string input_path_;
  std::vector<std::unique_ptr<std::fstream> > input_files_;
  std::unique_ptr<std::fstream> output_file_;
  std::unique_ptr<components::Indexer> indexer_;
};

}  // namespace util

#endif
