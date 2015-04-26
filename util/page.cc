#include <cctype>
#include <iostream>
#include <string>
#include "page.h"

namespace util {

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
