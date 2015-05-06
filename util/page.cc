#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
#include "page.h"

namespace util {
Page::Page() : Page(0) {};

Page::Page(unsigned page_id) : page_id_(page_id), page_url_(""), text_(""), length_(0), page_rank_(0), score_(0){};

Page::Page(unsigned page_id, const std::string& url, const std::string& text,
           double length, double page_rank)
    : page_id_(page_id), page_url_(url), text_(text), length_(length),
      page_rank_(page_rank), score_(0){};

Page::Page(unsigned page_id, const std::string& url, const std::string& text)
    : Page(page_id, url, text, 0, 0){};

void Page::UpdateWeights(unsigned term_id, double weight) {
  weights_[term_id] = weight;
}

void Page::CalculateLength() {
  length_ = 0;
  auto weight = weights_.begin();
  while (weight != weights_.end()) {
    length_ += weight->second * weight->second;
    ++weight;
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
