#ifndef _RI_2015_1_RANKING_PAGE_RANKER_H_
#define _RI_2015_1_RANKING_PAGE_RANKER_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include "../util/page.h"
#include "ranking_metadata.h"

namespace ranking {

class PageRanker {
 public:
  // Takes ownership of |ranking_metadata|
  PageRanker(RankingMetadata* ranking_metadata);

  // Build |ranking_metadata_| using |metadata_file_path|.
  PageRanker(const std::string& metadata_file_path);

  // Initializes the needed data.
  void Init();

  // Calculates the |page_rank| score of each object stored in
  // |ranking_metadata_|. |q| is the normalization constant between random
  // surfing and link surfing.
  void Rank(double q);

  // Returns the page rank score of |page|.
  double Rank(const ::util::Page& page, double q) const;

  // Writes the pages in |ranking_metadata_| back to their file.
  void UpdatePagesToFile();

 private:

  // Updates the |page_rank_| fields of every page stored in |ranking_metadata|.
  void UpdatePageRanks();

  // Calculates |web_graph_size_|.
  void CalculateWebGraphSize();

  // The number of nodes in the graph.
  unsigned web_graph_size_;

  // Metadata associated with the collection.
  std::unique_ptr<RankingMetadata> ranking_metadata_;

  // Stores the temporary page rank values of the pages in ;
  std::unordered_map<unsigned, double> temporary_page_ranks_;
};

}  // namespace ranking

#endif
