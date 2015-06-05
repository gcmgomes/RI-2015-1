#ifndef _RI_2015_1_RANKING_VECTOR_PREPROCESSOR_H_
#define _RI_2015_1_RANKING_VECTOR_PREPROCESSOR_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include "ranking_metadata.h"

namespace ranking {

class VectorPreprocessor {
 public:
  // Takes ownership of the supplied pointers.
  VectorPreprocessor(const std::string& index_file_path,
                     const std::string& anchor_index_file_path);

  const std::unordered_map<unsigned, double>& lengths() const {
    return lengths_;
  }

  const std::unordered_map<unsigned, double>& anchor_lengths() const {
    return anchor_lengths_;
  }
  // Properly preprocesses everything.
  void Preprocess();

  // Correctly calculates the length of each ::util::Page object.
  void PreprocessLengths();

 private:
  // File where the inverted index is stored.
  std::unique_ptr<std::fstream> index_file_;

  // File where the inverted index for anchor information is stored.
  std::unique_ptr<std::fstream> anchor_index_file_;

  // The mapping document_id -> length, without the needed sqrt call.
  std::unordered_map<unsigned, double> lengths_;

  std::unordered_map<unsigned, double> anchor_lengths_;
};

}  // namespace ranking

#endif
