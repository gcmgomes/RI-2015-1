#ifndef _RI_2015_1_PARSER_PAGE_KNOWLEDGE_H_
#define _RI_2015_1_PARSER_PAGE_KNOWLEDGE_H_

#include <iostream>
#include <memory>
#include <unordered_map>

namespace parsing {

struct PageInformation {
  PageInformation();
  PageInformation(unsigned id_, unsigned anchor_position_);

  unsigned id;
  unsigned anchor_position;
};

class PageKnowledge {
 public:
  PageKnowledge(const std::string& acceptable_pages);

  // Loads the pages listed in the index file. This is the same index file as
  // the one given to RICP.
  void LoadPageKnowledge(const std::string& acceptable_pages);

  std::unordered_map<std::string, PageInformation>& known_pages() {
    return known_pages_;
  }

 private:
  // Mapping url -> unsigned. The key should be normalized according to
  // ::util::PageUrl.
  std::unordered_map<std::string, PageInformation> known_pages_;
};

}  // namespace parsing

#endif
