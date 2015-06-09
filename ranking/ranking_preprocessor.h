#ifndef _RI_2015_1_RANKING_RANKING_PREPROCESSOR_H_
#define _RI_2015_1_RANKING_RANKING_PREPROCESSOR_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include "page_ranker.h"
#include "ranking_metadata.h"
#include "vector_preprocessor.h"
#include "../util/page.h"

namespace ranking {

class RankingPreprocessor {
 public:
  // Takes ownership of the supplied pointers.
  RankingPreprocessor(const std::string& input_metadata_file_path,
                      const std::string& output_metadata_file_path,
                      const std::string& index_file_path,
                      const std::string& anchor_index_file_path,
                      double random_surfer_q);

  // Properly preprocesses everything.
  void Preprocess(bool only_page_rank = false);

  // Correctly fill the link information present in |input_metadata_|.
  void PreprocessLinks();

  // Calculate the page ranks of each object in |input_metadata_| and writes the
  // updated values using |output_metadata_|.
  void PreprocessPageRanks();

  // Update the objects handled by |input_metadata_| to |output_metadata_|.
  void UpdatePages();

 private:
  // Container for ::util::Page objects being read.
  std::unique_ptr<RankingMetadata> input_metadata_;

  // Container for ::util::Page objects being written.
  std::unique_ptr<RankingMetadata> output_metadata_;

  // Object responsible for updating the page rank values of the ::util::Page
  // objects.
  std::unique_ptr<PageRanker> page_ranker_;

  std::unique_ptr<VectorPreprocessor> vector_preprocessor_;
};

}  // namespace ranking

#endif
