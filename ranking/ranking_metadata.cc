#include <cmath>
#include "ranking_metadata.h"

namespace ranking {

RankingMetadata::RankingMetadata(const std::string& metadata_file_path) {
  metadata_file_.open(metadata_file_path.c_str(), std::fstream::in |
                                                    std::fstream::out |
                                                    std::fstream::binary);
}

void RankingMetadata::WritePage(const std::unique_ptr<::util::Page>& page) {
  // Write the page identificator.
  metadata_file_.write(reinterpret_cast<const char*>(&(page->page_id_)),
                       sizeof(unsigned));

  // Write the amount of bytes needed to store the url.
  unsigned size = page->url().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  // Write the url itself.
  metadata_file_.write(page->url().c_str(), page->url().size());

  // Write the document length.
  metadata_file_.write(reinterpret_cast<const char*>(&(page->length_)),
                       sizeof(double));

  // Write the document PageRank.
  metadata_file_.write(reinterpret_cast<const char*>(&(page->page_rank_)),
                       sizeof(double));
}

std::unique_ptr<::util::Page> RankingMetadata::LoadPage() {
  unsigned size = 0;
  std::unique_ptr<::util::Page> page(new ::util::Page());

  // Retrieve the |page_id_| identifier.
  metadata_file_.read(reinterpret_cast<char*>(&(page->page_id_)),
                      sizeof(unsigned));

  // Retrieve the byte count of the url.
  metadata_file_.read(reinterpret_cast<char*>(&size), sizeof(unsigned));

  // Retrieve the url itself.
  page->url_.resize(size);
  metadata_file_.read(reinterpret_cast<char*>(&(page->url_[0])), size);

  // Retrieve the length.
  metadata_file_.read(reinterpret_cast<char*>(&(page->length_)),
                      sizeof(double));

  // Retrieve the page_rank.
  metadata_file_.read(reinterpret_cast<char*>(&(page->page_rank_)),
                      sizeof(double));

  return page;
}

void RankingMetadata::LoadPages() {
  while (!metadata_file_.eof() && metadata_file_.peek() != EOF) {
    auto page = std::move(LoadPage());
    pages_[page->page_id()] = (*page);
  }
}

void RankingMetadata::UpdatePagesWeights(
    unsigned term_id,
    const std::map<unsigned, std::vector<unsigned>>& occurrences) {
  auto occurrence = occurrences.begin();

  // Iterating through |occurrences| should be MUCH faster then iterating
  // through |pages_|.
  while (occurrence != occurrences.end()) {
    unsigned page_id = occurrence->first;

    // vector model weighting.
    double weight = (1 + log(occurrence->second.size()));
    weight *= log(static_cast<double>(pages_.size() / occurrences.size()));

    pages_[page_id].UpdateWeights(term_id, weight);
    ++occurrence;
  }
}

void RankingMetadata::CalculatePagesLengths() {
  auto page = pages_.begin();
  while (page != pages_.end()) {
    page->second.CalculateLength();
    ++page;
  }
}

}  // namespace ranking
