#ifndef _RI_2015_1_RANKING_RANKING_METADATA_H_
#define _RI_2015_1_RANKING_RANKING_METADATA_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include "../util/page.h"

namespace ranking {

class RankingMetadata {
 public:
  RankingMetadata(const std::string& metadata_file_path);

  std::unordered_map<unsigned, ::util::Page>& mutable_pages() {
    return pages_;
    }

  const std::unordered_map<unsigned, ::util::Page>& pages() const {
    return pages_;
  }

  // Write the information contained in |page| to |metadata_file_|.
  void WritePage(const std::unique_ptr<::util::Page>& page);

  // Load a single page object from |metadata_file_| and return it.
  std::unique_ptr<::util::Page> LoadPage();

  // Load the data in |metadata_file_| to |pages_|.
  void LoadPages();

  // Fills the |inlinks_| of each object in |pages_|.
  void FixInlinks();

  // Updates |pages_| to update the correct weights of each object, based on
  // |occurrences|.
  void UpdatePagesWeights(
      unsigned term_id,
      const std::map<unsigned, std::vector<unsigned>>& occurrences,
      bool is_anchor_weighting);

  void CalculatePagesLengths();

  // Writes the pages back to |metadata_file_|.
  void UpdatePagesToFile();

 private:
  // Stream where data will be read from/written to.
  std::fstream metadata_file_;

  // Stores the objects read from |metadata_file_|;
  std::unordered_map<unsigned, ::util::Page> pages_;
};

}  // namespace ranking

#endif
