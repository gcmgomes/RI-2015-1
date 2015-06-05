#ifndef _RI_2015_1_UTIL_PAGE_H_
#define _RI_2015_1_UTIL_PAGE_H_

#include <cstdlib>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include "page_url.h"

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

  Page(unsigned page_id, const std::string& url, const std::string& text,
       double length, double anchor_length, double page_rank);

  Page(unsigned page_id, const std::string& url, const std::string& text);

  unsigned page_id() const {
    return page_id_;
  }

  const PageUrl& page_url() const {
    return page_url_;
  }

  const std::string& url() const {
    return page_url_.url();
  }

  const std::string& text() const {
    return text_;
  }

  std::string& mutable_text() {
    return text_;
  }

  double& mutable_length() {
    return length_;
  }

  double& mutable_anchor_length() {
    return anchor_length_;
  }

  double length() const {
    return length_;
  }

  double anchor_length() const {
    return anchor_length_;
  }

  double& mutable_page_rank() {
    return page_rank_;
  }

  double page_rank() const {
    return page_rank_;
  }

  double score() const {
    return score_;
  }

  double& mutable_score() {
    return score_;
  }

  bool operator==(const Page& other_page) const {
    return page_id_ == other_page.page_id();
  }

  const std::unordered_map<unsigned, double>& weights() const {
    return weights_;
  }

  const std::unordered_map<unsigned, double>& anchor_weights() const {
    return anchor_weights_;
  }

  const std::unordered_set<unsigned>& outlinks() const {
    return outlinks_;
  }

  const std::unordered_set<unsigned>& inlinks() const {
    return inlinks_;
  }

  std::string ToString() const;

  std::string ToDebugString() const;

  // Returns the next token starting the search at |starting_position|, or the
  // empty string if the whole text has been processed.
  // A token is either:
  //  a string delimited by whitespaces or the beginning/end of the text;
  //  OR a number.
  std::string GetNextTokenFromText(unsigned& starting_position) const;

  // Update |weights|[term_id] based on |weight|.
  void UpdateWeights(unsigned term_id, double weight, bool is_anchor_weight);

  // Update the link information of this page.
  void UpdateLinks(unsigned page_id, bool is_inlink);

  // Calculate the |length| of the document based on the |weights_| mapping.
  void CalculateLength();

 private:
  // File id. Easier and cheaper to index then strings.
  unsigned page_id_;

  // Object's website.
  PageUrl page_url_;

  // Clean text. Consisting only of letters, numbers and whitespaces. No
  // diacritics allowed.
  std::string text_;

  // The length of the document.
  double length_;

  // Length of the document as described by it's anchor text.
  double anchor_length_;

  // The Page rank score associated with this webpage.
  double page_rank_;

  // The "quality" associated with each page object.
  double score_;

  // The occurrences of each term in the document. Set only during retrieval
  // phase.
  std::unordered_map<unsigned, double> weights_;

  // The occurrences of each term that describes this document in anchor text.
  // Set only during retrieval phase.
  std::unordered_map<unsigned, double> anchor_weights_;

  // The incoming links of this page. Set during the a preprocessing phase.
  std::unordered_set<unsigned> inlinks_;

  // The outgoing links of this page. Set during the parsing phase.
  std::unordered_set<unsigned> outlinks_;
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
