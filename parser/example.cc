#include "util/iconvpp.h"
#include "ricp/src/Document.h"
#include "ricp/src/CollectionReader.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <gumbo.h>
#include <algorithm>
#include <unicode/utypes.h>
#include <unicode/unistr.h>
#include <unicode/translit.h>

using namespace std;

static std::string cleantext(GumboNode* node) {
  if (node->type == GUMBO_NODE_TEXT) {
    return std::string(node->v.text.text);
  } else if (node->type == GUMBO_NODE_ELEMENT &&
             node->v.element.tag != GUMBO_TAG_SCRIPT &&
             node->v.element.tag != GUMBO_TAG_STYLE) {
    std::string contents = "";
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
      const std::string text = cleantext((GumboNode*)children->data[i]);
      if (i != 0 && !text.empty()) {
        contents.append(" ");
      }
      contents.append(text);
    }
    return contents;
  } else {
    return "";
  }
}

void desaxUTF8(std::string& str) {
  // UTF-8 std::string -> UTF-16 UnicodeString
  UnicodeString source = UnicodeString::fromUTF8(StringPiece(str));

  UErrorCode status = U_ZERO_ERROR;
  Transliterator* accentsConverter = Transliterator::createInstance(
      "NFD; [:M:] Remove; NFC", UTRANS_FORWARD, status);
  accentsConverter->transliterate(source);

  str.clear();
  source.toUTF8String(str);
}

void NiceUp(std::string& str) {
  desaxUTF8(str);
}

bool RemoveHeader(std::string& str) {
  unsigned pos = str.find("<!DOC");
  // either we are already at the beginning of the <html> tag or it doesn't
  // exist.
  if (!pos || pos == std::string::npos) {
    return false;
  }
  str.erase(0, pos - 1);
  return true;
}

void Parse(const std::string& html, std::string& dest) {
  GumboOutput* output = NULL;
  output = gumbo_parse(html.c_str());
  dest = cleantext(output->root);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << argv[0] << " [input directory] [index file]\n";
    return 0;
  }

  RICPNS::CollectionReader* reader =
      new RICPNS::CollectionReader(argv[1], argv[2]);
  RICPNS::Document doc;
  doc.clear();
  std::string str = "grüßen";
  NiceUp(str);
  // cout << str << endl;
  while (reader->getNextDocument(doc)) {
    std::string contents = doc.getText(), real_contents = "";
    //NiceUp(contents);
    //cout << contents << endl;
    bool res = RemoveHeader(contents);
    iconvpp::converter conv("UTF8", "ISO8859-1", false, 1000000);
    conv.convert(contents, real_contents);
    Parse(real_contents, contents);
     NiceUp(contents);
     cout << contents << endl;
    break;
  }
  delete reader;
}
