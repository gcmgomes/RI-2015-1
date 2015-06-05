#ifndef _RI_2015_1_PARSER_PAGE_KNOWLEDGE_H_
#define _RI_2015_1_PARSER_PAGE_KNOWLEDGE_H_

#include <iostream>
#include <memory>
#include <unordered_map>

namespace parsing {

struct PageInformation {
  PageInformation();
  PageInformation(unsigned id_, bool already_read_, unsigned anchor_position_);

  unsigned id;
  bool already_read;
  unsigned anchor_position;

  bool read() const {
    return already_read;
  }
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

  // Returns true if the page pointed by |page_url| has already been read.
  bool AlreadyRead(const std::string& page_url) const;

 private:
  // Mapping url -> unsigned. The key should be normalized according to
  // ::util::PageUrl.
  std::unordered_map<std::string, PageInformation> known_pages_;
};

}  // namespace parsing

#endif
