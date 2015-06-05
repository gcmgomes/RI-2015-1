#include <fstream>
#include "page_knowledge.h"
#include "../util/page_url.h"

namespace parsing {

PageInformation::PageInformation() : PageInformation(0, false, 0){};
PageInformation::PageInformation(unsigned id_, bool already_read_, unsigned anchor_position_)
    : id(id_), already_read(already_read_), anchor_position(anchor_position_){};

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
    if(!known_pages_.count(str)) {
      known_pages_[str] = PageInformation(i, false, 0);
      i++;
    }
    std::getline(input_file, str);
  }
}

bool PageKnowledge::AlreadyRead(const std::string& page_url) const {
  std::string url(page_url);
  NormalizeUrl(url);
  if(known_pages_.count(url)) {
    return known_pages_.at(url).read();
  }
  return false;
}

}  // namespace parsing
