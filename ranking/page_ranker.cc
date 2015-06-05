#include "page_ranker.h"

namespace ranking {

PageRanker::PageRanker(unsigned web_graph_size, double random_surfer_q)
    : web_graph_size_(web_graph_size), random_surfer_q_(random_surfer_q){};

double PageRanker::Rank(unsigned page_id) {
  if (web_graph_size_ == 0) {
    return 1.0;
  }
  auto inlink = inlink_graph_[page_id].begin();
  double E = random_surfer_q_ / web_graph_size_, page_rank = 0;
  while (inlink != inlink_graph_[page_id].end()) {
    unsigned linked_page_id = *inlink;
    double L = outlink_counts_[linked_page_id];
    page_rank += page_ranks_[linked_page_id] / L;
    ++inlink;
  }
  page_rank *= (1 - random_surfer_q_);
  page_rank += E;
  return page_rank;
}

void PageRanker::TemporaryUpdate(unsigned page_id, double page_rank) {
  if (page_ranks_[page_id] != page_rank) {
    temporary_page_ranks_[page_id] = page_rank;
  }
}

unsigned PageRanker::UpdatePageRanks() {
  unsigned changed_page_ranks = 0;
  auto page_rank = temporary_page_ranks_.begin();
  while (page_rank != temporary_page_ranks_.end()) {
    changed_page_ranks++;
    page_ranks_[page_rank->first] = page_rank->second;
    ++page_rank;
  }
  temporary_page_ranks_.clear();
  return changed_page_ranks;
}

}  // namespace ranking
