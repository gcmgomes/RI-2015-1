#include "page_ranker.h"

namespace ranking {

PageRanker::PageRanker(RankingMetadata* ranking_metadata)
    : web_graph_size_(0), ranking_metadata_(ranking_metadata){};

PageRanker::PageRanker(const std::string& metadata_file_path) {
  ranking_metadata_.reset(new RankingMetadata(metadata_file_path));
}

void PageRanker::Init() {
  ranking_metadata_->LoadPages();
  CalculateWebGraphSize();
}

void PageRanker::Rank(double q) {
  int dummy = 0;
  do {
    temporary_page_ranks_.clear();
    auto page = ranking_metadata_->mutable_pages().begin();
    while (page != ranking_metadata_->mutable_pages().end()) {
      double page_rank = Rank(page->second, q);
      if (page_rank != page->second.page_rank()) {
        temporary_page_ranks_[page->first] = page_rank;
      }
      ++page;
    }
    UpdatePageRanks();
  } while (!temporary_page_ranks_.empty());
}

double PageRanker::Rank(const ::util::Page& page, double q) const {
  if(web_graph_size_ == 0) {
    return page.page_rank();
  }
  auto inlink = page.inlinks().begin();
  double E = q / web_graph_size_, page_rank = 0;
  while (inlink != page.inlinks().end()) {
    unsigned page_id = *inlink;
    double L = ranking_metadata_->pages().at(page_id).outlinks().size();
    page_rank += ranking_metadata_->pages().at(page_id).page_rank() / L;
    ++inlink;
  }
  page_rank *= 1 - q;
  page_rank += E;
  return page_rank;
}

void PageRanker::UpdatePagesToFile() {
  ranking_metadata_->UpdatePagesToFile();
}

void PageRanker::UpdatePageRanks() {
  auto page_rank = temporary_page_ranks_.begin();
  while (page_rank != temporary_page_ranks_.end()) {
    ranking_metadata_->mutable_pages()
        .at(page_rank->first)
        .mutable_page_rank() = page_rank->second;
    ++page_rank;
  }
}

void PageRanker::CalculateWebGraphSize() {
  std::unordered_set<unsigned> nodes;
  auto page = ranking_metadata_->pages().begin();
  while (page != ranking_metadata_->pages().end()) {
    auto link = page->second.inlinks().begin();
    while (link != page->second.inlinks().end()) {
      nodes.insert(*link);
      ++link;
    }
    link = page->second.outlinks().begin();
    while (link != page->second.outlinks().end()) {
      nodes.insert(*link);
      ++link;
    }
    ++page;
  }
  web_graph_size_ = nodes.size();
}

}  // namespace ranking
