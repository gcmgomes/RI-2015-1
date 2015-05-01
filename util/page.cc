#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
#include "page.h"

namespace util {

Page::Page() : page_id_(0), url_(""), text_(""), length_(0), page_rank_(0){};

void Page::CalculateLength(
    const std::unordered_map<unsigned, unsigned>& frequencies) {
  length_ = 0;
  auto freq = frequencies.begin();
  while (freq != frequencies.end()) {
    length_ += freq->second * freq->second;
    freq++;
  }
  length_ = sqrt(length_);
}

std::string Page::GetNextTokenFromText(unsigned& starting_position) const {
  std::string token = "";
  while (starting_position < text_.size()) {
    const char& c = text_[starting_position];
    if (c == ' ') {
      if (!token.empty()) {
        return token;
      }
    } else {
      token += c;
    }
    starting_position++;
  }
  return token;
}

}  // namespace util
