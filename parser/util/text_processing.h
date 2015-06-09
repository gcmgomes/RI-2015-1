#ifndef _RI_2015_1_PARSER_UTIL_TEXT_PROCESSING_H_
#define _RI_2015_1_PARSER_UTIL_TEXT_PROCESSING_H_

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include "iconv.hpp"

namespace parsing {

// Check if |c| is neither a alphanumeric character nor a simple whitespace;
bool is_unwanted_char(char c) {
  return !::isalnum(c) && c != ' ';
}

void ConvertToUtf8(const std::string& charset, std::string& text) {
  if (charset == "utf-8") {
    return;
  }
  iconvpp::converter to_utf8("UTF-8", charset.c_str(), true, 1000000);
  std::string output;
  to_utf8.convert(text, output);
  text = output;
}

void RemoveDiacritics(std::string& text) {
  // "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖÙÚÛÜÝàáâãäåæçèéêëìíîïñòóôõöùúûüýÿ"
  // "AAAAAAECEEEEIIIIDNOOOOOUUUUYaaaaaaeceeeeiiiinooooouuuuyy";
}

// Performs some lightweight nicening on a pages |title|.
void TreatTitle(std::string& title) {
  iconvpp::converter to_ascii("ascii", "UTF-8", true, 1000000);
  std::string output;
  to_ascii.convert(title, output);
  title = output;
}

// Performs diacritical removal and useless character substitution.
void TreatText(std::string& text) {
  iconvpp::converter to_ascii("ascii//translit", "UTF-8", true, 1000000);
  std::string output;
  to_ascii.convert(text, output);
  text.clear();
  unsigned i = 0;
  while (i < output.size()) {
    if (output[i] == '\'' || output[i] == '`' || output[i] == '^' ||
        output[i] == '~') {
    } else if (is_unwanted_char(output[i])) {
      text += ' ';
    } else {
      text += std::tolower(output[i]);
    }
    i++;
  }
}

}  // namespace parsing

#endif
