#include <fstream>
#include "page_knowledge.h"
#include "../util/page_url.h"

namespace parsing {

PageInformation::PageInformation() : PageInformation(0,0){};
PageInformation::PageInformation(unsigned id_, unsigned anchor_position_)
    : id(id_), anchor_position(anchor_position_){};

PageKnowledge::PageKnowledge(const std::string& acceptable_pages) {
  LoadPageKnowledge(acceptable_pages);
}

static void NormalizeUrl(std::string& url) {
  ::util::PageUrl aux_url(url);
  url = aux_url.url();
}

void PageKnowledge::LoadPageKnowledge(const std::string& acceptable_pages) {
  std::fstream input_file;
  input_file.open(acceptable_pages.c_str(), std::fstream::in);
  unsigned i = 0;
  while (!input_file.eof() && input_file.peek() != EOF) {
    std::string str = "";
    std::getline(input_file, str, ' ');
    NormalizeUrl(str);
    known_pages_[str] = PageInformation(i, 0);
    std::getline(input_file, str);
    i++;
  }
}

}  // namespace parsing
