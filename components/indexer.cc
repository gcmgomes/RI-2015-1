#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cmath>
#include "indexer.h"

namespace components {

std::unique_ptr<Compressor> Indexer::compressor_ = nullptr;
unsigned Indexer::frequency_compression_threshold = 1;

IndexMetadata::IndexMetadata(unsigned term_, unsigned document_,
                             unsigned document_count_,
                             std::streampos document_count_file_position_)
    : term(term_), document(document_), document_count(document_count_),
      document_count_file_position(document_count_file_position_),
      original_positions_size(0), compressed_positions_size(0),
      immutable_size(0){};

Indexer::Indexer() : Indexer(new Compressor()){};

Indexer::Indexer(Compressor* compressor) {
  Indexer::compressor_.reset(compressor);
  this->index_metadata_.reset(nullptr);
}

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

  file->write(reinterpret_cast<const char*>(&tuple->document),
              sizeof(unsigned));

  file->write(reinterpret_cast<const char*>(&tuple->frequency),
              sizeof(unsigned));

  // Update the size required for the header information.
  index_metadata_->immutable_size = immutable + (4 * sizeof(unsigned));
}

void Indexer::WritePositions(std::unique_ptr<std::fstream>& file) {
  // If we don't use compression, the increase is just for the positions
  // themselves
  index_metadata_->original_positions_size +=
      index_metadata_->current_document_positions.size() * sizeof(unsigned);
  // If we are using a compressor, write to the index using it.
  if (compressor_ != nullptr &&
      index_metadata_->current_document_positions.size() >
          Indexer::frequency_compression_threshold) {
    // We have to compress it to save up some space.
    std::string encoded_positions = "";
    compressor_->Encode(index_metadata_->current_document_positions,
                        encoded_positions);

    // Write the bytes used to store the encoded positions.
    unsigned size = encoded_positions.size();
    file->write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

    // Write the encoded positions themselves.
    file->write(reinterpret_cast<const char*>(&encoded_positions[0]),
                encoded_positions.size() * sizeof(char));

    // We have to write the byte count for the encoded positions as well.
    index_metadata_->compressed_positions_size +=
        encoded_positions.size() * sizeof(char) + sizeof(unsigned);
  } else {
    unsigned frequency = index_metadata_->current_document_positions.size();
    file->write(reinterpret_cast<const char*>(
                    &index_metadata_->current_document_positions[0]),
                frequency * sizeof(unsigned));
    index_metadata_->compressed_positions_size += frequency * sizeof(unsigned);
  }
  index_metadata_->current_document_positions.clear();
}

void Indexer::UpdateDocumentCount(std::unique_ptr<std::fstream>& file) {
  // We have to write the position vector to |file|
  WritePositions(file);

  // Get position to where we will continue writing.
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
  // file  and rebuild the metadata.
  if (tuple->term != index_metadata_->term) {
    UpdateDocumentCount(file);
    WriteTermHeader(tuple, file);
  }

  // If we find a different document, we have to write the new frequency, the
  // new document_id and update the metadata.
  if (tuple->document != index_metadata_->document) {
    WritePositions(file);

    index_metadata_->document = tuple->document;
    index_metadata_->document_count++;

    // Write document header information.
    file->write(reinterpret_cast<const char*>(&tuple->document),
                sizeof(unsigned));
    file->write(reinterpret_cast<const char*>(&tuple->frequency),
                sizeof(unsigned));
    // We have to update the byte count neede for this write.
    index_metadata_->immutable_size += 2 * sizeof(unsigned);
  }

  index_metadata_->current_document_positions.push_back(tuple->position);
}

void Indexer::FinishIndex(std::unique_ptr<std::fstream>& file) {
  if (index_metadata_ != nullptr) {
    UpdateDocumentCount(file);
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
                                unsigned frequency,
                                std::vector<unsigned>& positions) {
  positions.clear();
  // We have a compressor available, use it to decompress!
  if (compressor_ != nullptr &&
      frequency > Indexer::frequency_compression_threshold) {
    // Read the bytes used to store the encoded positions.
    unsigned size = 0;
    file->read(reinterpret_cast<char*>(&size), sizeof(unsigned));

    // Read the encoded positions themselves.
    std::string encoded_positions(size, 0);
    file->read(reinterpret_cast<char*>(&encoded_positions[0]),
               size * sizeof(char));
    // We have to decompress it to get the original vector.
    compressor_->Decode(frequency, encoded_positions, positions);
  } else {
    positions.assign(frequency, 0);
    file->read(reinterpret_cast<char*>(&positions[0]),
               frequency * sizeof(unsigned));
  }
}

void Indexer::GetNextEntry(std::unique_ptr<std::fstream>& input_file,
                           IndexEntry& entry) {
  entry.term_ = GetUnsignedFromBin(input_file.get());
  entry.occurrences_.clear();
  unsigned doc_count = GetUnsignedFromBin(input_file.get());
  while (doc_count) {
    unsigned doc_id = GetUnsignedFromBin(input_file.get());
    unsigned frequency = GetUnsignedFromBin(input_file.get());
    entry.occurrences_[doc_id] = std::vector<unsigned>();
    GetPositionVector(input_file, frequency, entry.occurrences_[doc_id]);
    doc_count--;
  }
}

}  // namespace components
