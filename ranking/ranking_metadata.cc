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
  size = page->weights().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  // We have to write the weights to avoid building them every time we load the ranker.
  auto i = page->weights().begin();
  while(i != page->weights().end()) {
    size = i->first;
    double value = i->second;
    metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));
    metadata_file_.write(reinterpret_cast<const char*>(&value), sizeof(double));
    ++i;
  }

  size = page->anchor_weights().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  i = page->anchor_weights().begin();
  while(i != page->anchor_weights().end()) {
    size = i->first;
    double value = i->second;
    metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));
    metadata_file_.write(reinterpret_cast<const char*>(&value), sizeof(double));
    ++i;
  }

  size = page->outlinks().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  auto j = page->outlinks().begin();
  while(j != page->outlinks().end()) {
    size = *j;
    metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));
    ++j;
  }

  size = page->inlinks().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  j = page->inlinks().begin();
  while(j != page->inlinks().end()) {
    size = *j;
    metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));
    ++j;
  }
}

std::unique_ptr<::util::Page> RankingMetadata::LoadPage() {
  unsigned size = 0, page_id = 0;
  double length = 0, page_rank = 0;
  std::string url = "";

  // Retrieve the |page_id_| identifier.
  metadata_file_.read(reinterpret_cast<char*>(&(page_id)), sizeof(unsigned));

  // Retrieve the byte count of the url.
  metadata_file_.read(reinterpret_cast<char*>(&size), sizeof(unsigned));

  // Retrieve the url itself.
  url.resize(size);
  metadata_file_.read(reinterpret_cast<char*>(&url[0]), size);

  // Retrieve the length.
  metadata_file_.read(reinterpret_cast<char*>(&length), sizeof(double));

  // Retrieve the page_rank.
  metadata_file_.read(reinterpret_cast<char*>(&page_rank), sizeof(double));

  std::unique_ptr<::util::Page> page(
      new ::util::Page(page_id, url, "", length, page_rank));
 
  bool is_anchor_weighting = false;
  do {
    // Retrieve the amount of weights.
    metadata_file_.read(reinterpret_cast<char*>(&size), sizeof(unsigned));
    while(size) {
      unsigned weight_id = 0;
      double weight_value = 0;
      metadata_file_.read(reinterpret_cast<char*>(&weight_id), sizeof(unsigned));
      metadata_file_.read(reinterpret_cast<char*>(&weight_value), sizeof(double));
      page->UpdateWeights(weight_id, weight_value, is_anchor_weighting);
      size--;
    }

    is_anchor_weighting = !is_anchor_weighting;
  } while(is_anchor_weighting);

  bool is_inlink = false;
  do {
    // Retrieve the amount of links.
    metadata_file_.read(reinterpret_cast<char*>(&size), sizeof(unsigned));
    while(size) {
      unsigned page_id = 0;
      metadata_file_.read(reinterpret_cast<char*>(&page_id), sizeof(unsigned));
      page->UpdateLinks(page_id, is_inlink);
      size--;
    }

    is_inlink = !is_inlink;
  } while(is_inlink);
  return page;
}

void RankingMetadata::LoadPages() {
  while (!metadata_file_.eof() && metadata_file_.peek() != EOF) {
    auto page = std::move(LoadPage());
    pages_[page->page_id()] = (*page);
  }
}

void RankingMetadata::FixInlinks() {
  bool is_inlink = true;
   auto page = pages_.begin();
   while(page != pages_.end()) {
     auto link_id = page->second.outlinks().begin();
     while(link_id != page->second.outlinks().end()) {
       pages_[*link_id].UpdateLinks(page->first, is_inlink);
       ++link_id;
     }
     ++page;
   }
}

void RankingMetadata::UpdatePagesWeights(
    unsigned term_id,
    const std::map<unsigned, std::vector<unsigned>>& occurrences,
    bool is_anchor_weighting) {
  auto occurrence = occurrences.begin();

  // Iterating through |occurrences| should be MUCH faster then iterating
  // through |pages_|.
  while (occurrence != occurrences.end()) {
    unsigned page_id = occurrence->first;

    // vector model weighting.
    double weight = (1 + log(occurrence->second.size()));
    weight *= log(1 + static_cast<double>(pages_.size() / occurrences.size()));

    pages_[page_id].UpdateWeights(term_id, weight, is_anchor_weighting);
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

void RankingMetadata::UpdatePagesToFile() {
  metadata_file_.seekp(metadata_file_.beg);
  metadata_file_.seekg(metadata_file_.beg);
  auto page = pages_.begin();
  while(page != pages_.end()) {
    std::unique_ptr<::util::Page> ptr_page(&page->second);
    WritePage(ptr_page);
    ptr_page.release();
    ++page;
  }
}

}  // namespace ranking
