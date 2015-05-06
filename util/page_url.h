#ifndef _RI_2015_1_UTIL_PAGE_URL_H_
#define _RI_2015_1_UTIL_PAGE_URL_H_

#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>

namespace util {

class PageUrl {
 public:
  PageUrl(const std::string& url);

  bool is_absolute() const {
    return is_absolute_;
  }

  const std::string& url() const {
    return url_;
  }

  const std::vector<std::string>& tokens() const {
    return tokens_;
  }

  // Breaks |url_| into tokens separated by '/'s and stores them in |tokens_|.
  void TokenizeUrl();

  // Returns true if |url| is a relative path, otherwise false.
  static bool IsRelative(const std::string& url);

  // Enumeration for movement actions.
  enum MoveAction {
    STAY = 0,
    MOVE_DOWN = 1,
    MOVE_TO_ROOT = 2,
    MOVE_UP = 3
  };

  // Processes |tokens| according to the path specified by |href|.
  void RelativeMove(const std::string& href);

 private:
  // True iff |url| is represented as a absolute path.
  bool is_absolute_;

  // Object's website.
  std::string url_;
  std::vector<std::string> tokens_;
};

}  // namespace util

#endif
