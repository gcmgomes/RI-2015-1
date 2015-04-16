#ifndef _RI_2015_1_PARSER_UTIL_TEXT_PROCESSING_H_
#define _RI_2015_1_PARSER_UTIL_TEXT_PROCESSING_H_

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string>
#include "htmlEntities.h"

namespace parsing {
namespace util {

// Check if |c| is neither a alphanumeric character nor a simple whitespace;
bool is_unwanted_char(char c) {
  return !::isalnum(c) && c != ' ';
}

void ConvertToUtf8(std::string& document) {
  char* buffer = new char[document.size() + 1];
  decode_html_entities_utf8(buffer, document.c_str());
  document = buffer;
  delete[] buffer;
}

static void RemoveDiacriticsInternal(const char* src, char* dst, int idx) {
  // "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ"
  const char* tr =
      "AAAAAAECEEEEIIIIDNOOOOOx0UUUUYPsaaaaaaeceeeeiiiiOnooooo/0uuuuypy";
  while (*src != 0) {
    u_char ch = *src;
    if (ch == 0x0A) {
      dst[idx++] = (char)' ';
      ++src;
      continue;
    }
    if (ch < 32 || (ch > 126 && ch < 192)) {
      ++src;
      continue;
    }
    if (ch >= 192) {
      ch = tr[ch - 192];
    }
    dst[idx++] = (char)ch;
    ++src;
  }
  dst[idx] = (char)0;
}

void RemoveDiacritics(std::string& text) {
  const char* src = text.c_str();
  char* tmp = new char[text.size() + 1];
  RemoveDiacriticsInternal(src, tmp, 0);
  text = tmp;
  std::transform(text.begin(), text.end(), text.begin(), ::tolower);
  delete[] tmp;
}

// Performs diacritical removal and useless character substitution.
void TreatText(std::string& text) {
  RemoveDiacritics(text);

  // Replace useless characters with ' '. We can't outright remove, otherwise
  // "a\nb" would become "ab", which we don't want.
  std::replace_if(text.begin(), text.end(), is_unwanted_char, ' ');
}

}  // namespace util
}  // namespace parsing

#endif
