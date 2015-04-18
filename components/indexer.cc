#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cmath>
#include "indexer.h"

namespace components {

Indexer::Indexer() {
  current_index_entry_.reset(nullptr);
}

void Indexer::EntryHeader(const util::Tuple* tuple) {
  // Delete previous entry.
  current_index_entry_.reset(new IndexEntry());

  // Allocate entry term.
  current_index_entry_->term_ = tuple->term;
}

void Indexer::WriteTuple(const util::Tuple* tuple,
                         std::unique_ptr<std::fstream>& file) {
  // Lazy initialization.
  if (current_index_entry_ == nullptr) {
    EntryHeader(tuple);
  }

  // If we find a different term, we have to wrap up |current_index_entry_|
  if (tuple->term != current_index_entry_->term()) {
    WriteEntry(file, current_index_entry_.get());
    EntryHeader(tuple);
  }

  // We have to push the new occurence, regardless of what's happening.
  current_index_entry_->occurrences_[tuple->document].push_back(
      tuple->position);
}

void Indexer::FinishIndex(std::unique_ptr<std::fstream>& file) {
  if (current_index_entry_ != nullptr) {
    WriteEntry(file, current_index_entry_.get());
    current_index_entry_.reset();
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
  entry.occurrences_.clear();
  unsigned doc_count = GetUnsignedFromBin(input_file.get());
  while (doc_count) {
    unsigned doc_id = GetUnsignedFromBin(input_file.get());
    unsigned frequency = GetUnsignedFromBin(input_file.get());
    entry.occurrences_[doc_id].assign(frequency, 0);
    unsigned i = 0;
    while (frequency) {
      unsigned position = GetUnsignedFromBin(input_file.get());
      entry.occurrences_[doc_id][i++] = position;
      frequency--;
    }
    doc_count--;
  }
}

void Indexer::WriteEntry(std::unique_ptr<std::fstream>& output_file,
                         const IndexEntry* entry) {
  // Write term id.
  unsigned term = entry->term();
  output_file->write(reinterpret_cast<const char*>(&term),
                     sizeof(unsigned));

  // Write amount of documents.
  unsigned doc_count = entry->occurrences().size();
  output_file->write(reinterpret_cast<const char*>(&doc_count),
                     sizeof(unsigned));

  auto i = entry->occurrences_.begin();
  while (i != entry->occurrences_.end()) {
    // Write doc_id.
    output_file->write(reinterpret_cast<const char*>(&i->first),
                       sizeof(unsigned));

    // Write frequency.
    unsigned frequency = i->second.size();
    output_file->write(reinterpret_cast<const char*>(&frequency),
                       sizeof(unsigned));

    auto j = i->second.begin();
    while (j != i->second.end()) {
      unsigned pos = *j;
      output_file->write(reinterpret_cast<const char*>(&pos), sizeof(unsigned));
      ++j;
    }
    ++i;
  }
}

}  // namespace components
