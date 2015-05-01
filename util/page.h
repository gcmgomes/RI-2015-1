#ifndef _RI_2015_1_UTIL_PAGE_H_
#define _RI_2015_1_UTIL_PAGE_H_

#include <iostream>
#include <cstdlib>
#include <unordered_map>

namespace parsing {

class Parser;

}  // namespace parsing

namespace ranking {

class RankingMetadata;

}  // namespace ranking

namespace util {

class Page {
 public:
  friend class ::parsing::Parser;
  friend class ::ranking::RankingMetadata;

  Page();

  unsigned page_id() const {
    return page_id_;
  }

  const std::string& url() const {
    return url_;
  }

  const std::string& text() const {
    return text_;
  }

  double length() const {
    return length_;
  }

  double page_rank() const {
    return page_rank_;
  }

  std::string ToString() const {
    std::string str = "";
    char i[128];
    sprintf(i, "%u ", page_id_);
    str += i;
    str += url_ + ' ';
    str += text_ + ' ';
    sprintf(i, "%lf ", length_);
    str += i;
    sprintf(i, "%lf ", page_rank_);
    str += i;
    return str;
  }

  // Returns the next token starting the search at |starting_position|, or the
  // empty string if the whole text has been processed.
  // A token is either:
  //  a string delimited by whitespaces or the beginning/end of the text;
  //  OR a number.
  std::string GetNextTokenFromText(unsigned& starting_position) const;

  // Calculate the |length| of the document based on the |frequencies| mapping.
  void CalculateLength(
      const std::unordered_map<unsigned, unsigned>& frequencies);

 private:
  Page(unsigned page_id, const std::string& url, const std::string& text,
       double length, double page_rank)
      : page_id_(page_id), url_(url), text_(text), length_(length),
        page_rank_(page_rank){};

  Page(unsigned page_id, const std::string& url, const std::string& text)
      : Page(page_id, url, text, 0, 0){};

  // File id. Easier and cheaper to index then strings.
  unsigned page_id_;

  // Object's website.
  std::string url_;

  // Clean text. Consisting only of letters, numbers and whitespaces. No
  // diacritics allowed.
  std::string text_;

  // The length of the document.
  double length_;

  // The Page rank score associated with this webpage.
  double page_rank_;
};

}  // namespace util

#endif
