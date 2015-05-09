#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
#include "page.h"

namespace util {
Page::Page() : Page(0){};

Page::Page(unsigned page_id)
    : page_id_(page_id), page_url_(""), text_(""), length_(0), page_rank_(1),
      score_(0){};

Page::Page(unsigned page_id, const std::string& url, const std::string& text,
           double length, double page_rank)
    : page_id_(page_id), page_url_(url), text_(text), length_(length),
      page_rank_(page_rank), score_(0){};

Page::Page(unsigned page_id, const std::string& url, const std::string& text)
    : Page(page_id, url, text, 0, 1){};

void Page::UpdateWeights(unsigned term_id, double weight,
                         bool is_anchor_weight) {
  if (is_anchor_weight) {
    anchor_weights_[term_id] = weight;
  } else {
    weights_[term_id] = weight;
  }
}

void Page::UpdateLinks(unsigned page_id, bool is_inlink) {
  if(page_id == page_id_) {
    return;
  }
  if (is_inlink) {
    inlinks_.insert(page_id);
  } else {
    outlinks_.insert(page_id);
  }
}

void Page::CalculateLength() {
  length_ = 0;
  auto weight = weights_.begin();
  while (weight != weights_.end()) {
    length_ += weight->second * weight->second;
    ++weight;
  }
  length_ = sqrt(length_);

  anchor_length_ = 0;
  weight = anchor_weights_.begin();
  while (weight != anchor_weights_.end()) {
    anchor_length_ += weight->second * weight->second;
    ++weight;
  }
  anchor_length_ = sqrt(anchor_length_);
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
