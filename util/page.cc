#include <cctype>
#include <cmath>
#include <iostream>
#include <string>
#include "page.h"

namespace util {
Page::Page() : Page(0){};

Page::Page(unsigned page_id) : Page(page_id, "", "", 0, 0, 1){};

Page::Page(unsigned page_id, const std::string& url, const std::string& text,
           double length, double page_rank)
    : Page(page_id, url, text, length, 0, page_rank){};

Page::Page(unsigned page_id, const std::string& url, const std::string& text,
           double length, double anchor_length, double page_rank)
    : page_id_(page_id), page_url_(url), text_(text), length_(length),
      anchor_length_(anchor_length), page_rank_(page_rank), score_(0),
      weights_(std::unordered_map<unsigned, double>()),
      anchor_weights_(std::unordered_map<unsigned, double>()),
      inlinks_(std::unordered_set<unsigned>()),
      outlinks_(std::unordered_set<unsigned>()){};

Page::Page(unsigned page_id, const std::string& url, const std::string& text)
    : Page(page_id, url, text, 0, 0, 1){};

void Page::UpdateWeights(unsigned term_id, double weight,
                         bool is_anchor_weight) {
  if (is_anchor_weight) {
    anchor_weights_[term_id] = weight;
  } else {
    weights_[term_id] = weight;
  }
}

void Page::UpdateLinks(unsigned page_id, bool is_inlink) {
  if (page_id == page_id_) {
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

std::string Page::ToString() const {
  std::string str = "", url(page_url_.url());
  char i[128];
  sprintf(i, "%u ", page_id_);
  str += i;
  str += url + ' ';
  str += text_ + ' ';
  sprintf(i, "%lf ", length_);
  str += i;
  sprintf(i, "%lf ", anchor_length_);
  str += i;
  sprintf(i, "%lf ", page_rank_);
  str += i;
  sprintf(i, "%lf", score_);
  str += i;
  return str;
}

std::string Page::ToDebugString() const {
  std::string str = "", url(page_url_.url());
  char i[128] = "";
  sprintf(i, "%u ", page_id_);
  str += i;
  str += url + ' ';
  str += text_ + ' ';
  sprintf(i, "%lf ", length_);
  str += i;
  sprintf(i, "%lf ", anchor_length_);
  str += i;
  sprintf(i, "%lf ", page_rank_);
  str += i;
  sprintf(i, "%lf", score_);
  str += i;
  str += " | ";
  auto w = weights_.begin();
  while (w != weights_.end()) {
    sprintf(i, "%u ", w->first);
    str += i;
    sprintf(i, "%lf ", w->second);
    str += i;
    ++w;
  }
  str += " | ";
  w = anchor_weights_.begin();
  while (w != anchor_weights_.end()) {
    sprintf(i, "%u ", w->first);
    str += i;
    sprintf(i, "%lf ", w->second);
    str += i;
    ++w;
  }

  str += " | ";
  auto l = outlinks_.begin();
  while (l != outlinks_.end()) {
    sprintf(i, "%u ", *l);
    str += i;
    ++l;
  }
  str += " | ";
  l = inlinks_.begin();
  while (l != inlinks_.end()) {
    sprintf(i, "%u ", *l);
    str += i;
    ++l;
  }

  return str;
}
}  // namespace util
