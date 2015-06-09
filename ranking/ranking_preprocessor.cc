#include "ranking_preprocessor.h"
#include <cmath>

namespace ranking {

RankingPreprocessor::RankingPreprocessor(
    const std::string& input_metadata_file_path,
    const std::string& output_metadata_file_path,
    const std::string& index_file_path,
    const std::string& anchor_index_file_path, double random_surfer_q)
    : input_metadata_(new RankingMetadata(input_metadata_file_path)),
      output_metadata_(new RankingMetadata(output_metadata_file_path, true)),
      page_ranker_(new PageRanker(0, random_surfer_q)),
      vector_preprocessor_(
          new VectorPreprocessor(index_file_path, anchor_index_file_path)){};

static void FillLink(
    const std::unique_ptr<::util::Page>& page,
    std::unordered_map<unsigned, std::unordered_set<unsigned>>& inlink_graph) {
  auto outlink = page->outlinks().begin();
  while (outlink != page->outlinks().end()) {
    inlink_graph[*outlink].insert(page->page_id());
    ++outlink;
  }
}

static void FillLink(
    std::unordered_map<unsigned, std::unordered_set<unsigned>>& inlink_graph,
    const std::unique_ptr<::util::Page>& page) {
  if (!inlink_graph.count(page->page_id())) {
    return;
  }
  bool is_inlink = true;
  auto inlink = inlink_graph[page->page_id()].begin();
  while (inlink != inlink_graph[page->page_id()].end()) {
    page->UpdateLinks(*inlink, is_inlink);
    ++inlink;
  }
}

void RankingPreprocessor::Preprocess(bool only_page_rank)  {
  PreprocessLinks();

  PreprocessPageRanks();

  if(!only_page_rank) {
    vector_preprocessor_->Preprocess();
  }
  UpdatePages();
}

void RankingPreprocessor::PreprocessLinks() {
  unsigned processed_pages = 0;
  // Fill the in-memory graph structure. It shouldn't be gigantic.
  while (!input_metadata_->eof()) {
    std::unique_ptr<::util::Page> page = input_metadata_->LoadPage();
    FillLink(page, page_ranker_->mutable_inlink_graph());
    page_ranker_->mutable_page_ranks()[page->page_id()] = page->page_rank();
    page_ranker_->mutable_outlink_counts()[page->page_id()] =
        page->outlinks().size();
    if ((processed_pages % 1000) == 0) {
      std::cout << "\rProcessed pages: " << processed_pages;
      fflush(stdout);
    }
    processed_pages++;
  }
  page_ranker_->mutable_web_graph_size() = processed_pages;
  std::cout << "\rProcessed pages: " << processed_pages;
  std::cout << std::endl;
  input_metadata_->ReturnToBegin();

  // Unload the graph into their correct ::util::Page objects and sores them
  // with |output_metadata_|.
  while (!input_metadata_->eof()) {
    processed_pages--;
    std::unique_ptr<::util::Page> page = input_metadata_->LoadPage();
    output_metadata_->WritePage(page);
  }
  std::cout << std::endl;
}

void RankingPreprocessor::PreprocessPageRanks() {
  unsigned page_rank_delta = 1;
  while (page_rank_delta > 0) {
    auto page_node = page_ranker_->mutable_page_ranks().begin();
    while (page_node != page_ranker_->mutable_page_ranks().end()) {
      unsigned page_id = page_node->first;
      double page_rank = page_ranker_->Rank(page_id);
      page_ranker_->TemporaryUpdate(page_id, page_rank);
      ++page_node;
    }
    page_rank_delta = page_ranker_->UpdatePageRanks();
    std::cout << "\rStable PageRank count: "
              << page_ranker_->mutable_web_graph_size() - page_rank_delta;
    fflush(stdout);
  }
  std::cout << std::endl;
}

void RankingPreprocessor::UpdatePages() {
  input_metadata_->ReturnToBegin();

  while (!input_metadata_->eof()) {
    auto page = input_metadata_->LoadPage();

    // Update vector model data.
    if (vector_preprocessor_->lengths().count(page->page_id())) {
      page->mutable_length() =
          sqrt(vector_preprocessor_->lengths().at(page->page_id()));
    }

    if (vector_preprocessor_->anchor_lengths().count(page->page_id())) {
      page->mutable_anchor_length() =
          sqrt(vector_preprocessor_->anchor_lengths().at(page->page_id()));
    }

    // Update page rank data.
    page->mutable_page_rank() =
        page_ranker_->mutable_page_ranks()[page->page_id()];

    // Update link structure data.
    FillLink(page_ranker_->mutable_inlink_graph(), page);

    output_metadata_->WritePage(page);
  }
}

}  // namespace ranking
