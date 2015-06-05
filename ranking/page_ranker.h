#ifndef _RI_2015_1_RANKING_PAGE_RANKER_H_
#define _RI_2015_1_RANKING_PAGE_RANKER_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "../util/page.h"
#include "ranking_metadata.h"

namespace ranking {

class PageRanker {
 public:
  PageRanker(unsigned web_graph_size, double random_surfer_q);

  unsigned& mutable_web_graph_size() {
    return web_graph_size_;
  }

  std::unordered_map<unsigned, double>& mutable_page_ranks() {
    return page_ranks_;
  }

  std::unordered_map<unsigned, unsigned>& mutable_outlink_counts() {
    return outlink_counts_;
  }

  std::unordered_map<unsigned, std::unordered_set<unsigned>>& mutable_inlink_graph() {
    return inlink_graph_;
  }

  // Returns the page rank score of position |page_id| of |inlink_graph_|.
  double Rank(unsigned page_id);

  // Updates |temporary_page_ranks_|.
  void TemporaryUpdate(unsigned page_id, double page_rank);

  // Transfers the page ranks from |temporary_page_ranks_| to |page_ranks|.
  // Returns true if any changes were made, false otherwise.
  unsigned UpdatePageRanks();

 private:
  // The number of nodes in the graph.
  unsigned web_graph_size_;

  // The random surfer parameter.
  double random_surfer_q_;

  // The mapping page_id -> page_rank of the current scores.
  std::unordered_map<unsigned, double> page_ranks_;

  // The mapping page_id -> page_rank of the temporary scores.
  std::unordered_map<unsigned, double> temporary_page_ranks_;

  // The mapping page_id -> #outlinks.
  std::unordered_map<unsigned, unsigned> outlink_counts_;

  // The graph representing the inlink relationships.
  std::unordered_map<unsigned, std::unordered_set<unsigned>> inlink_graph_;
};

}  // namespace ranking

#endif
