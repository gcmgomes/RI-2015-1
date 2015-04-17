#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cmath>
#include "indexer.h"

namespace components {

IndexMetadata::IndexMetadata(unsigned term_, unsigned document_,
                             unsigned document_count_,
                             unsigned document_count_file_position_)
    : term(term_), document(document_), document_count(document_count_),
      document_count_file_position(document_count_file_position_){};

Indexer::Indexer() {
  index_metadata_.reset(nullptr);
}

void Indexer::WriteTermHeader(const util::Tuple* tuple,
                              std::unique_ptr<std::fstream>& file) {
  // Helper for file writes.
  unsigned zero = 0;

  index_metadata_.reset(new IndexMetadata(tuple->term, tuple->document, 1, 0));
  // Write the term id we are indexing.
  file->write(reinterpret_cast<const char*>(&tuple->term), sizeof(unsigned));

  // Save where we will need to seek to overwrite |document_count| later on.
  index_metadata_->document_count_file_position = (unsigned)file->tellp();
  file->write(reinterpret_cast<const char*>(&zero), sizeof(unsigned));
  file->write(reinterpret_cast<const char*>(&tuple->document),
              sizeof(unsigned));
  file->write(reinterpret_cast<const char*>(&tuple->frequency),
              sizeof(unsigned));
}

void Indexer::UpdateDocumentCount(std::unique_ptr<std::fstream>& file) {
  // Get correct write placement.
  unsigned current_pos = file->tellp();
  file->seekp(index_metadata_->document_count_file_position);
  file->write(reinterpret_cast<const char*>(&index_metadata_->document_count),
              sizeof(unsigned));
  // Return to EOF.
  file->seekp(current_pos);
}

void Indexer::WriteTuple(const util::Tuple* tuple,
                         std::unique_ptr<std::fstream>& file) {
  // Lazy initialization.
  if (index_metadata_ == nullptr) {
    WriteTermHeader(tuple, file);
  }

  // If we find a different term, we have to wrap up |document_count| in the
  // file and rebuild the metadata.
  if (tuple->term != index_metadata_->term) {
    UpdateDocumentCount(file);
    WriteTermHeader(tuple, file);
  }

  // If we find a different document, we have to write the new frequency, the
  // new document_id and update the metadata.
  if (tuple->document != index_metadata_->document) {
    index_metadata_->document = tuple->document;
    index_metadata_->document_count++;
    file->write(reinterpret_cast<const char*>(&tuple->document),
                sizeof(unsigned));
    file->write(reinterpret_cast<const char*>(&tuple->frequency),
                sizeof(unsigned));
  }

  // We have to write the new position, regardless of what's happening.
  file->write(reinterpret_cast<const char*>(&tuple->position),
              sizeof(unsigned));
}

void Indexer::FinishIndex(std::unique_ptr<std::fstream>& file) {
  if (index_metadata_ != nullptr) {
    UpdateDocumentCount(file);
  }
}

static unsigned GetUnsignedFromBin(std::fstream* input_file) {
  unsigned ret_val = 0;
  input_file->read(reinterpret_cast<char*>(&ret_val), sizeof(unsigned));
  return ret_val;
}

void Indexer::ConvertIndexToText(const std::string& binary_input_file_path,
                                 const std::string& text_filepath) {
  std::fstream input_file(binary_input_file_path.c_str(),
                          std::fstream::binary | std::fstream::in);
  std::fstream output_file(text_filepath.c_str(), std::fstream::out);

  while (!input_file.eof() && input_file.peek() != EOF) {
    unsigned term = GetUnsignedFromBin(&input_file);
    unsigned doc_count = GetUnsignedFromBin(&input_file);
    output_file << term << " | " << doc_count << " {";
    while (doc_count) {
      unsigned doc_id = GetUnsignedFromBin(&input_file);
      unsigned frequency = GetUnsignedFromBin(&input_file);
      output_file << "[" << doc_id << ", " << frequency << "(";
      while (frequency) {
        unsigned position = GetUnsignedFromBin(&input_file);
        frequency--;
        output_file << position;
        if (frequency) {
          output_file << ", ";
        }
      }
      doc_count--;
      output_file << ")]";
      if (doc_count) {
        output_file << ", ";
      }
    }
    output_file << "}" << std::endl;
  }
  input_file.close();
  output_file.close();
}

void Indexer::GetNextEntry(std::unique_ptr<std::fstream>& input_file,
                           IndexEntry& entry) {
  entry.term_ = GetUnsignedFromBin(input_file.get());
  entry.occurences_.clear();
  unsigned doc_count = GetUnsignedFromBin(input_file.get());
  while (doc_count) {
    unsigned doc_id = GetUnsignedFromBin(input_file.get());
    unsigned frequency = GetUnsignedFromBin(input_file.get());
    entry.occurences_[doc_id].assign(frequency, 0);
    unsigned i = 0;
    while (frequency) {
      unsigned position = GetUnsignedFromBin(input_file.get());
      entry.occurences_[doc_id][i++] = position;
      frequency--;
    }
    doc_count--;
  }
}

}  // namespace components
