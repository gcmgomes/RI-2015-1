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

  // Creates the file. CAREFUL, this will erase the file pointed by
  // |metadata_file_path|.
  RankingMetadata(const std::string& metadata_file_path, bool clean_file);

  std::unordered_map<unsigned, std::pair<double, double>>&
  mutable_page_lengths() {
    return page_lengths_;
  }

  const std::unordered_map<unsigned, std::pair<double, double>>& page_lengths()
      const {
    return page_lengths_;
  }

  std::unordered_map<unsigned, unsigned>& mutable_pages() {
    return pages_;
  }

  const std::unordered_map<unsigned, unsigned>& pages() const {
    return pages_;
  }

  // Write the information contained in |page| to |metadata_file_|.
  void WritePage(const std::unique_ptr<::util::Page>& page);

  // Returns true if the end of the input file has been reached, false
  // otherwise.
  bool eof();

  // Loads a single page object from |metadata_file_| and returns it.
  std::unique_ptr<::util::Page> LoadPage();

  // Loads a single page object identified by |page_id| from |metadata_file_|
  // and returns it.
  std::unique_ptr<::util::Page> LoadPage(unsigned page_id);

  // Load the data in |metadata_file_| to |pages_|.
  void LoadPages();

  // Returns the read/write pointers to the beginning of the file.
  void ReturnToBegin();

 private:
  // Stream where data will be read from/written to.
  std::fstream metadata_file_;

  // Stores the positions of objects read from |metadata_file_|;
  std::unordered_map<unsigned, unsigned> pages_;

  // Stores the lengths of each ::util::Page object.
  std::unordered_map<unsigned, std::pair<double, double>> page_lengths_;
};

}  // namespace ranking

#endif
