#ifndef _RI_2015_1_COMPONENTS_INDEXER_H_
#define _RI_2015_1_COMPONENTS_INDEXER_H_

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include "compressor.h"
#include "index_entry.h"
#include "../util/tuple.h"

namespace components {

struct IndexMetadata {
  IndexMetadata(unsigned term_, unsigned document_, unsigned document_count_,
                std::streampos document_count_file_position_);
  unsigned term;
  unsigned document;
  unsigned document_count;
  std::vector<unsigned> current_document_positions;
  std::streampos document_count_file_position;
  unsigned long long original_positions_size;
  unsigned long long compressed_positions_size;
  unsigned long long immutable_size;
};

class Indexer {
 public:
  Indexer();

  // Writes |tuple| to |file| in index format.
  void WriteTuple(const util::Tuple* tuple,
                  std::unique_ptr<std::fstream>& file);

  // Updates the document count for the last time.
  void FinishIndex(std::unique_ptr<std::fstream>& file);

  // Converts |binary_input_file_path| into human-readable
  // |text_output_file_path|.
  static void ConvertIndexToText(const std::string& binary_input_file_path,
                                 const std::string& text_output_file_path);

  static void GetNextEntry(std::unique_ptr<std::fstream>& input_file,
                           IndexEntry& entry);

 private:
  // Writes the header for the current index entry.
  void WriteTermHeader(const util::Tuple* tuple,
                       std::unique_ptr<std::fstream>& file);

  // Writes the position vector to |file|
  void WritePositions(std::unique_ptr<std::fstream>& file);

  // Loads the position vector from |file| and stores it in |positions|.
  static void GetPositionVector(std::unique_ptr<std::fstream>& file,
                                std::vector<unsigned>& positions);

  // Updates the |document_count| of the current index entry.
  void UpdateDocumentCount(std::unique_ptr<std::fstream>& file);

  static Compressor compressor_;

  std::unique_ptr<IndexMetadata> index_metadata_;
};
}  // namespace components
#endif
