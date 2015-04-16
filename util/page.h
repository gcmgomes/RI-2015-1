#ifndef _RI_2015_1_UTIL_PAGE_H_
#define _RI_2015_1_UTIL_PAGE_H_

#include <iostream>
#include <cstdlib>

namespace parsing {

class Parser;

}  // namespace parsing

namespace util {

class Page {
 public:
  friend class parsing::Parser;

  const std::string& url() {
    return url_;
  }
  const std::string& text() {
    return text_;
  }

  // Returns the next token starting the search at |starting_position|, or the
  // empty string if the whole text has been processed.
  // A token is either:
  //  a string delimited by whitespaces or the beginning/end of the text;
  //  OR a digit. We save digits instead of numbers to reduce the amount of
  //  unique terms.
  std::string GetNextTokenFromText(unsigned& starting_position) const;

 private:
  Page(const std::string& url, const std::string& text)
      : url_(url), text_(text){};
  // Object's website.
  std::string url_;

  // Clean text. Consisting only of letters, numbers and whitespaces. No
  // diacritics allowed.
  std::string text_;
};

}  // namespace util

#endif
