#ifndef _RI_2015_1_COMPONENTS_INDEXER_H_
#define _RI_2015_1_COMPONENTS_INDEXER_H_

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include "index_entry.h"
#include "../util/tuple.h"

namespace components {

class Indexer {
 public:
  Indexer();
  // Writes |tuple| to |file| in index format.
  void WriteTuple(const util::Tuple* tuple, std::unique_ptr<std::fstream>& file);

  // Updates the document count for the last time.
  void FinishIndex(std::unique_ptr<std::fstream>& file);

  // Converts |binary_input_file_path| into human-readable
  // |text_output_file_path|.
  static void ConvertIndexToText(const std::string& binary_input_file_path,
                                 const std::string& text_output_file_path);

  static void GetNextEntry(std::unique_ptr<std::fstream>& input_file,
                           IndexEntry& entry);

  static void WriteEntry(std::unique_ptr<std::fstream>& output_file,
                         const IndexEntry* entry);

 private:
  // Writes the header for the current index entry.
  void EntryHeader(const util::Tuple* tuple);

  std::unique_ptr<IndexEntry> current_index_entry_;
};

}  // namespace components

#endif
