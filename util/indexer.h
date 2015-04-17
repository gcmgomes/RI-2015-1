#ifndef _RI_2015_1_UTIL_INDEXER_H_
#define _RI_2015_1_UTIL_INDEXER_H_

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include "tuple.h"

namespace util {

struct IndexMetadata {
  IndexMetadata(unsigned term_, unsigned document_,
                               unsigned document_count_,
                               unsigned document_count_file_position_);

  unsigned term;
  unsigned document;
  unsigned document_count;
  unsigned document_count_file_position;
};

class Indexer {
 public:
  Indexer();
  // Writes |tuple| to |file| in index format.
  void WriteTuple(const Tuple* tuple, std::unique_ptr<std::fstream>& file);

  // Updates the document count for the last time.
  void FinishIndex(std::unique_ptr<std::fstream>& file);

  // Converts |binary_input_file_path| into human-readable
  // |text_output_file_path|.
  static void ConvertIndexToText(const std::string& binary_input_file_path,
                                 const std::string& text_output_file_path);

 private:
  // Writes the header for the current index entry.
  void WriteTermHeader(const Tuple* tuple, std::unique_ptr<std::fstream>& file);

  // Updates the |document_count| of the current index entry.
  void UpdateDocumentCount(std::unique_ptr<std::fstream>& file);

  std::unique_ptr<IndexMetadata> index_metadata_;
};

}  // namespace util

#endif