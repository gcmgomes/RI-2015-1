#ifndef _RI_2015_1_UTIL_PAGE_H_
#define _RI_2015_1_UTIL_PAGE_H_

#include <iostream>
#include <cstdlib>
#include <functional>
#include <map>
#include <unordered_map>

namespace components {

class Retriever;

}  // namespace components

namespace parsing {

class Parser;

}  // namespace parsing

namespace ranking {

class Ranker;
class RankingMetadata;

}  // namespace ranking

namespace util {

class Page {
 public:
  friend class ::components::Retriever;
  friend class ::parsing::Parser;
  friend class ::ranking::Ranker;
  friend class ::ranking::RankingMetadata;

  Page();

  Page(unsigned page_id);

  Page(unsigned page_id, const std::string& url, const std::string& text,
       double length, double page_rank);

  Page(unsigned page_id, const std::string& url, const std::string& text);

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

  double score() const {
    return score_;
  }

  bool operator==(const Page& other_page) const {
    return page_id_ == other_page.page_id();
  }

  const std::unordered_map<unsigned, double>& weights() const {
    return weights_;
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
    sprintf(i, "%lf", score_);
    str += i;
    return str;
  }

  // Returns the next token starting the search at |starting_position|, or the
  // empty string if the whole text has been processed.
  // A token is either:
  //  a string delimited by whitespaces or the beginning/end of the text;
  //  OR a number.
  std::string GetNextTokenFromText(unsigned& starting_position) const;

  // Update |weights|[term_id] based on |weight|.
  void UpdateWeights(unsigned term_id, double weight);

  // Calculate the |length| of the document based on the |weights_| mapping.
  void CalculateLength();

 private:
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

  // The "quality" associated with each page object.
  double score_;

  // The occurrences of each term in the document. Set only during retrieval
  // phase.
  std::unordered_map<unsigned, double> weights_;
};

struct PageHash {
  size_t operator()(const Page& p) const {
    return std::hash<unsigned>()(p.page_id());
  };
  bool operator()(const Page& p1, const Page& p2) const {
    return p1.score() > p2.score();
  }
};

}  // namespace util

#endif
