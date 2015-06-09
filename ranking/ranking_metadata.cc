#include <cmath>
#include "ranking_metadata.h"

namespace ranking {

RankingMetadata::RankingMetadata(const std::string& metadata_file_path)
    : RankingMetadata(metadata_file_path, false){};

RankingMetadata::RankingMetadata(const std::string& metadata_file_path,
                                 bool clean_file) {
  if (clean_file) {
    metadata_file_.open(metadata_file_path.c_str(), std::fstream::out);
    metadata_file_.flush();
    metadata_file_.close();
  }
  metadata_file_.open(metadata_file_path.c_str(), std::fstream::in |
                                                      std::fstream::out |
                                                      std::fstream::binary);
}

void RankingMetadata::WritePage(const std::unique_ptr<::util::Page>& page) {
  // Write the page identificator.
  // std::cout << "wrote: " << page->ToDebugString() << std::endl;
  metadata_file_.write(reinterpret_cast<const char*>(&(page->page_id_)),
                       sizeof(unsigned));

  unsigned size = page->url().size(),
           text_size = std::min( (unsigned) 256, (unsigned)page->text().size());
  // Write the amount of bytes needed to store the url.
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  // Write the url itself.
  metadata_file_.write(page->url().c_str(), page->url().size());

  // Write the amount of bytes needed to store the title.
  size = page->title().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  // Write the title itself.
  metadata_file_.write(page->title().c_str(), page->title().size());

  // Write the amount of bytes needed to store the text.
  metadata_file_.write(reinterpret_cast<const char*>(&text_size), sizeof(unsigned));

  // Write the text itself.
  metadata_file_.write(page->text().c_str(), text_size);

  // Write the document length.
  metadata_file_.write(reinterpret_cast<const char*>(&(page->length_)),
                       sizeof(double));

  metadata_file_.write(reinterpret_cast<const char*>(&(page->anchor_length_)),
                       sizeof(double));

  // Write the document PageRank.
  metadata_file_.write(reinterpret_cast<const char*>(&(page->page_rank_)),
                       sizeof(double));
  size = page->weights().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  // We have to write the weights to avoid building them every time we load the
  // ranker.
  auto i = page->weights().begin();
  while (i != page->weights().end()) {
    size = i->first;
    double value = i->second;
    metadata_file_.write(reinterpret_cast<const char*>(&size),
                         sizeof(unsigned));
    metadata_file_.write(reinterpret_cast<const char*>(&value), sizeof(double));
    ++i;
  }

  size = page->anchor_weights().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  i = page->anchor_weights().begin();
  while (i != page->anchor_weights().end()) {
    size = i->first;
    double value = i->second;
    metadata_file_.write(reinterpret_cast<const char*>(&size),
                         sizeof(unsigned));
    metadata_file_.write(reinterpret_cast<const char*>(&value), sizeof(double));
    ++i;
  }

  size = page->outlinks().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  auto j = page->outlinks().begin();
  while (j != page->outlinks().end()) {
    size = *j;
    metadata_file_.write(reinterpret_cast<const char*>(&size),
                         sizeof(unsigned));
    ++j;
  }

  size = page->inlinks().size();
  metadata_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));

  j = page->inlinks().begin();
  while (j != page->inlinks().end()) {
    size = *j;
    metadata_file_.write(reinterpret_cast<const char*>(&size),
                         sizeof(unsigned));
    ++j;
  }
}

bool RankingMetadata::eof() {
  return metadata_file_.eof() || metadata_file_.peek() == EOF;
}

std::unique_ptr<::util::Page> RankingMetadata::LoadPage() {
  unsigned size = 0, page_id = 0;
  double length = 0, anchor_length, page_rank = 0;
  std::string url = "", text = "", title;

  // Retrieve the |page_id_| identifier.
  metadata_file_.read(reinterpret_cast<char*>(&(page_id)), sizeof(unsigned));

  // Retrieve the byte count of the url.
  metadata_file_.read(reinterpret_cast<char*>(&size), sizeof(unsigned));

  // Retrieve the url itself.
  url.resize(size);
  metadata_file_.read(reinterpret_cast<char*>(&url[0]), size);

  // Retrieve the byte count of the saved title.
  metadata_file_.read(reinterpret_cast<char*>(&size), sizeof(unsigned));

  // Retrieve the title itself.
  title.resize(size);
  metadata_file_.read(reinterpret_cast<char*>(&title[0]), size);

  // Retrieve the byte count of the saved text.
  metadata_file_.read(reinterpret_cast<char*>(&size), sizeof(unsigned));

  // Retrieve the text itself.
  text.resize(size);
  metadata_file_.read(reinterpret_cast<char*>(&text[0]), size);

  // Retrieve the length.
  metadata_file_.read(reinterpret_cast<char*>(&length), sizeof(double));

  // Retrieve the anchor length.
  metadata_file_.read(reinterpret_cast<char*>(&anchor_length), sizeof(double));

  // Retrieve the page_rank.
  metadata_file_.read(reinterpret_cast<char*>(&page_rank), sizeof(double));

  std::unique_ptr<::util::Page> page(
      new ::util::Page(page_id, url, title, text, length, anchor_length, page_rank));

  bool is_anchor_weighting = false;
  do {
    // Retrieve the amount of weights.
    metadata_file_.read(reinterpret_cast<char*>(&size), sizeof(unsigned));
    while (size) {
      unsigned weight_id = 0;
      double weight_value = 0;
      metadata_file_.read(reinterpret_cast<char*>(&weight_id),
                          sizeof(unsigned));
      metadata_file_.read(reinterpret_cast<char*>(&weight_value),
                          sizeof(double));
      page->UpdateWeights(weight_id, weight_value, is_anchor_weighting);
      size--;
    }

    is_anchor_weighting = !is_anchor_weighting;
  } while (is_anchor_weighting);

  bool is_inlink = false;
  do {
    // Retrieve the amount of links.
    metadata_file_.read(reinterpret_cast<char*>(&size), sizeof(unsigned));
    while (size) {
      unsigned page_id = 0;
      metadata_file_.read(reinterpret_cast<char*>(&page_id), sizeof(unsigned));
      page->UpdateLinks(page_id, is_inlink);
      size--;
    }

    is_inlink = !is_inlink;
  } while (is_inlink);
  return page;
}

std::unique_ptr<::util::Page> RankingMetadata::LoadPage(unsigned page_id) {
  metadata_file_.seekg(pages_[page_id]);
  return LoadPage();
}

void RankingMetadata::LoadPages() {
  while (!metadata_file_.eof() && metadata_file_.peek() != EOF) {
    unsigned pos = metadata_file_.tellg();
    auto page = std::move(LoadPage());
    pages_[page->page_id()] = pos;
    page_lengths_[page->page_id()] =
        std::make_pair(page->length(), page->anchor_length());
  }
}

void RankingMetadata::ReturnToBegin() {
  metadata_file_.seekg(0, metadata_file_.beg);
  metadata_file_.seekp(0, metadata_file_.beg);
}

}  // namespace ranking
