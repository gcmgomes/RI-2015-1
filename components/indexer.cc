#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cmath>
#include "indexer.h"

namespace components {

Compressor Indexer::compressor_ = Compressor();

IndexMetadata::IndexMetadata(unsigned term_, unsigned document_,
                             unsigned document_count_,
                             std::streampos document_count_file_position_)
    : term(term_), document(document_), document_count(document_count_),
      total_frequency(0),
      document_count_file_position(document_count_file_position_),
      original_positions_size(0), compressed_positions_size(0),
      immutable_size(0){};

Indexer::Indexer() : index_metadata_(nullptr){};

void Indexer::WriteTermHeader(const util::Tuple* tuple,
                              std::unique_ptr<std::fstream>& file) {
  // Helper for file writes.
  unsigned zero = 0;
  unsigned long long original_positions = 0;
  unsigned long long compressed_positions = 0;
  unsigned long long immutable = 0;

  if (index_metadata_ != nullptr) {
    original_positions = index_metadata_->original_positions_size;
    compressed_positions = index_metadata_->compressed_positions_size;
    immutable = index_metadata_->immutable_size;
  }

  index_metadata_.reset(new IndexMetadata(tuple->term, tuple->document, 1, 0));

  index_metadata_->original_positions_size = original_positions;
  index_metadata_->compressed_positions_size = compressed_positions;
  index_metadata_->immutable_size = immutable;

  // Write the term id we are indexing.
  file->write(reinterpret_cast<const char*>(&tuple->term), sizeof(unsigned));

  // Save where we will need to seekp to overwrite |document_count| later on.
  index_metadata_->document_count_file_position = file->tellp();
  file->write(reinterpret_cast<const char*>(&zero), sizeof(unsigned));

  // Save where we will need to seekp to overwrite |total_frequency| later on.
  file->write(reinterpret_cast<const char*>(&zero), sizeof(unsigned));

  // Update the size required for the header information.i
  index_metadata_->immutable_size = immutable + (1 * sizeof(unsigned));
}

void Indexer::WritePositions(std::unique_ptr<std::fstream>& file) {
  // If we don't use compression, the increase is due to the positions
  // themselves.
  int frequency = index_metadata_->current_document_positions.size();
  index_metadata_->total_frequency += (unsigned) frequency;

  index_metadata_->original_positions_size += frequency * sizeof(unsigned);

  // Write document header information.
  file->write(reinterpret_cast<const char*>(&index_metadata_->document),
              sizeof(unsigned));

  // We have to update the byte count needed for these writes.
  index_metadata_->immutable_size += 2 * sizeof(unsigned);

  // We have to compress to see if we can save up some space.
  std::string encoded_positions = "";
  compressor_.Encode(index_metadata_->current_document_positions,
                     encoded_positions);

  unsigned encoded_size = encoded_positions.size() + sizeof(unsigned);
  unsigned standard_size =
      index_metadata_->current_document_positions.size() * sizeof(unsigned);

  // If compression is worth it, do it!
  if (encoded_size < standard_size) {
    // We use a negative frequency to mark a compressed positional list.
    frequency = -frequency;
    file->write(reinterpret_cast<const char*>(&frequency), sizeof(int));

    // Write the bytes used to store the encoded positions.
    unsigned size = encoded_positions.size();
    file->write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

    // Write the encoded positions themselves.
    file->write(reinterpret_cast<const char*>(&encoded_positions[0]),
                encoded_positions.size() * sizeof(char));

    // We have to write the byte count for the encoded positions as well.
    index_metadata_->compressed_positions_size +=
        encoded_positions.size() * sizeof(char) + sizeof(unsigned);
  }
  // Otherwise just write them as if compression never happened.
  else {
    file->write(reinterpret_cast<const char*>(&frequency), sizeof(int));
    file->write(reinterpret_cast<const char*>(
                    &index_metadata_->current_document_positions[0]),
                frequency * sizeof(unsigned));
    index_metadata_->compressed_positions_size += frequency * sizeof(unsigned);
  }
  index_metadata_->current_document_positions.clear();
}

void Indexer::UpdateMetadata(std::unique_ptr<std::fstream>& file) {
  // We have to write the position vector to |file|
  WritePositions(file);

  // Get position to where we will continue writing.
  unsigned current_pos = file->tellp();

  file->seekp(index_metadata_->document_count_file_position);
  // Write document count.
  file->write(reinterpret_cast<const char*>(&index_metadata_->document_count),
              sizeof(unsigned));

  // Write total collection frequency.
  file->write(reinterpret_cast<const char*>(&index_metadata_->total_frequency),
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
  // file  and rebuild the metadata.
  if (tuple->term != index_metadata_->term) {
    UpdateMetadata(file);
    WriteTermHeader(tuple, file);
  }

  // If we find a different document, we have to write the new frequency, the
  // new document_id and update the metadata.
  if (tuple->document != index_metadata_->document) {
    WritePositions(file);

    index_metadata_->document = tuple->document;
    index_metadata_->document_count++;
  }

  index_metadata_->current_document_positions.push_back(tuple->position);
}

void Indexer::FinishIndex(std::unique_ptr<std::fstream>& file) {
  if (index_metadata_ != nullptr) {
    UpdateMetadata(file);
    std::cout << "Immutable needed data:     "
              << index_metadata_->immutable_size << std::endl;
    std::cout << "Original Positions Size:   "
              << index_metadata_->original_positions_size << std::endl;
    std::cout << "Compressed Positions Size: "
              << index_metadata_->compressed_positions_size << std::endl;
  }
}

static unsigned GetUnsignedFromBin(std::fstream* input_file) {
  unsigned ret_val = 0;
  input_file->read(reinterpret_cast<char*>(&ret_val), sizeof(unsigned));
  return ret_val;
}
void Indexer::ConvertIndexToText(const std::string& binary_input_file_path,
                                 const std::string& text_filepath) {
  std::unique_ptr<std::fstream> input_file(new std::fstream());
  input_file->open(binary_input_file_path.c_str(),
                   std::fstream::binary | std::fstream::in);

  std::unique_ptr<std::fstream> output_file(new std::fstream());
  output_file->open(text_filepath.c_str(), std::fstream::out);
  while (!input_file->eof() && input_file->peek() != EOF) {
    IndexEntry entry;
    Indexer::GetNextEntry(input_file, entry);
    (*output_file) << entry.ToString() << std::endl;
  }
  input_file->close();
  output_file->close();
}

void Indexer::GetPositionVector(std::unique_ptr<std::fstream>& file,
                                std::vector<unsigned>& positions) {
  positions.clear();
  int frequency = 0;
  bool decompress = false;
  file->read(reinterpret_cast<char*>(&frequency), sizeof(int));
  if (frequency < 0) {
    decompress = true;
    frequency = -frequency;
  }

  // We have found ourselves a compressed list. Decompress it!
  if (decompress) {
    // Read the bytes used to store the encoded positions.
    unsigned size = 0;
    file->read(reinterpret_cast<char*>(&size), sizeof(unsigned));

    // Read the encoded positions themselves.
    std::string encoded_positions(size, 0);
    file->read(reinterpret_cast<char*>(&encoded_positions[0]),
               size * sizeof(char));
    // We have to decompress it to get the original vector.
    compressor_.Decode(frequency, encoded_positions, positions);
  }
  // The positions are regularly stored, just read them.
  else {
    positions.assign(frequency, 0);
    file->read(reinterpret_cast<char*>(&positions[0]),
               frequency * sizeof(unsigned));
  }
}

bool Indexer::GetNextEntry(std::unique_ptr<std::fstream>& input_file,
                           IndexEntry& entry) {
  if(input_file->eof() || input_file->peek() == EOF) {
    return false;
  }
  entry.term_ = GetUnsignedFromBin(input_file.get());
  entry.occurrences_.clear();
  unsigned doc_count = GetUnsignedFromBin(input_file.get());
  entry.total_frequency_ = GetUnsignedFromBin(input_file.get());
  while (doc_count) {
    unsigned doc_id = GetUnsignedFromBin(input_file.get());
    entry.occurrences_[doc_id] = std::vector<unsigned>();
    GetPositionVector(input_file, entry.occurrences_[doc_id]);
    doc_count--;
  }
  return true;
}

}  // namespace components
