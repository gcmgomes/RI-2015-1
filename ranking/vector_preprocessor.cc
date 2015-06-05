#include <cmath>
#include "vector_preprocessor.h"
#include "../components/indexer.h"

namespace ranking {

VectorPreprocessor::VectorPreprocessor(
    const std::string& index_file_path,
    const std::string& anchor_index_file_path) {
  index_file_.reset(new std::fstream(
      index_file_path.c_str(),
      std::fstream::in | std::fstream::out | std::fstream::binary));
  anchor_index_file_.reset(new std::fstream(
      anchor_index_file_path.c_str(),
      std::fstream::in | std::fstream::out | std::fstream::binary));
};

void VectorPreprocessor::Preprocess() {
  PreprocessLengths();
}

// Returns the amount of unique documents present in the wanted |index|.
static unsigned GetDocumentsCount(
    const std::unique_ptr<std::fstream>& index_file) {
  std::unordered_set<unsigned> known_documents(1000000);
  while (!index_file->eof() && index_file->peek() != EOF) {
    ::components::IndexEntry entry;
    ::components::Indexer::GetNextEntry(index_file, entry);
    auto i = entry.occurrences().begin();
    while (i != entry.occurrences().end()) {
      known_documents.insert(i->first);
      ++i;
    }
  }
  index_file->seekg(index_file->beg);
  return known_documents.size();
}

static void IncrementLengthComputing(
    const ::components::IndexEntry& entry,
    std::unordered_map<unsigned, double>& lengths, unsigned N) {
  auto occurrence = entry.occurrences().begin();
  double n = N, n_i = entry.occurrences().size();
  double idf = n / n_i;
  while (occurrence != entry.occurrences().end()) {
    // Modify RankingMetadata to hold the size of the dataset.
    double tf = occurrence->second.size();
    double weight = (1 + log10(tf) * log10(idf));
    lengths[occurrence->first] += weight * weight;
    ++occurrence;
  }
}

void VectorPreprocessor::PreprocessLengths() {
  unsigned doc_count = 0, anchor_doc_count = 0;

  doc_count = GetDocumentsCount(index_file_);
  anchor_doc_count = GetDocumentsCount(anchor_index_file_);

  unsigned processed_entries = 0;
  while (index_file_->peek() != EOF && !index_file_->eof()) {
    ::components::IndexEntry entry;
    ::components::Indexer::GetNextEntry(index_file_, entry);
    IncrementLengthComputing(entry, lengths_, doc_count);

    if ((processed_entries % 1000) == 0) {
      std::cout << "\rProcessed entries for length computation: "
                << processed_entries;
      fflush(stdout);
    }
    processed_entries++;
  }

  auto i = lengths_.begin();
  while (i != lengths_.end()) {
    i->second = sqrt(i->second);
    ++i;
  }

  std::cout << std::endl;
  processed_entries = 0;
  while (anchor_index_file_->peek() != EOF && !anchor_index_file_->eof()) {
    ::components::IndexEntry entry;
    ::components::Indexer::GetNextEntry(anchor_index_file_, entry);
    IncrementLengthComputing(entry, anchor_lengths_, anchor_doc_count);

    /*  if ((processed_entries % 1000) == 0) {
        std::cout << "\rProcessed entries for anchor length computation: "
                  << processed_entries;
        fflush(stdout);
      }*/
    processed_entries++;
  }

  std::cout << std::endl;
  i = anchor_lengths_.begin();
  while (i != anchor_lengths_.end()) {
    i->second = sqrt(i->second);
    ++i;
  }
}

}  // namespace ranking
