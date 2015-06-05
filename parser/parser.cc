#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <gumbo.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "parser.h"
#include "util/text_processing.h"
#include "../util/tuple.h"
#include "../util/page_url.h"

namespace parsing {

// Returns the charset encoding used by |source|.
static std::string Charset(const std::string& source) {
  size_t pos = source.find("charset=charset=");
  if (pos != std::string::npos) {
    pos += 16;
  } else {
    pos = source.find("charset=");
    if (pos == std::string::npos) {
      return "";
    } else {
      pos += 8;
    }
  }
  std::string charset = "";
  while (pos < source.size() &&
         (isalnum(source.at(pos)) || source.at(pos) == '-' ||
          source.at(pos) == '_')) {
    charset += source.at(pos);
    ++pos;
  }
  return charset;
}

// Remove useless information present in |source|'s header.
static bool RemoveHeader(std::string& source) {
  unsigned pos = source.find("<");
  if (pos && pos != std::string::npos) {
    source.erase(0, pos - 1);
    return true;
  }
  return false;
}

bool Parser::Convert(std::string& document) {
  if (!RemoveHeader(document)) {
    return false;
  }
  std::string charset = Charset(document);
  std::transform(charset.begin(), charset.end(), charset.begin(), ::tolower);
  if (charset.size() < 3 || charset == "auto" || charset == "x-user-defined" ||
      charset == "user-defined" || charset == "none" || charset == "charset" ||
      charset == "replace" || charset == "lang_charset" ||
      charset == "page_charset" || charset == "_charset" ||
      charset == "string" || charset == "iso-072972apa-3" || charset == "all" ||
      charset == "euc" || charset == "iso-639-1") {
    charset = "iso-8859-1";
  } else if (charset == "iso" || charset == "iso-" || charset == "iso-8859" ||
             charset == "iso-3339-1" || charset == "iso-8859-6-i" ||
             charset == "iso-8869-1" || charset == "iso-8859-1html" ||
             charset == "iso-8559-1" || charset == "en-iso-8859-1" ||
             charset == "iso88591" || charset == "8859-1" ||
             charset == "iso-88591" || charset == "western" ||
             charset == "english" || charset == "iso-859-1" ||
             charset == "iso8859_1" || charset == "iso-8909-1" ||
             charset == "iso-8059-1" || charset == "iso-8859-15latin9" ||
             charset == "iso-8895-1" || charset == "iso-8959-1" ||
             charset == "is0-8859-1" || charset == "iso-8859-0" ||
             charset == "iso-99591" || charset == "iso-9959-1" ||
             charset == "windows-8859-1" || charset == "pt_br" ||
             charset == "pt-br" || charset == "iso-8859-1--" ||
             charset == "ltr" || charset == "iso-88591-" ||
             charset == "is0-8859-2" || charset == "irso-8859-1" ||
             charset == "3diso-8859-1" || charset == "iso-885-1") {
    charset = "iso-8859-1";
  } else if (charset == "utf-11" || charset == "default" ||
             charset == "uft-8" || charset == "utf" || charset == "utf-1--" ||
             charset == "utf-10" || charset == "unicode" ||
             charset == "iso-10646" || charset == "utf-6" ||
             charset == "utf-5" || charset == "urf-8" || charset == "utf-4" ||
             charset == "iso-uft-8" || charset == "uto-8" ||
             charset == "0x0416" || charset == "iso-utf-8" ||
             charset == "utf8_general_ci" || charset == "unicode-1-1-utf-8" ||
             charset == "http" || charset == "utf8-unicode-ci-iso8859-5" ||
             charset ==
                 "utf-"
                 "8scripttypetextjavascriptsrchttpwwwbrainyquotecomlinkquotebrj"
                 "sscript") {
    charset = "utf-8";
  } else if (charset == "win-1250" || charset == "iso-1250") {
    charset = "windows-1250";
  } else if (charset == "win-1251" || charset == "iso-1251") {
    charset = "windows-1251";
  } else if (charset == "latin-1" || charset == "latin1_swedish_ci") {
    charset = "latin1";
  } else if (charset == "latin-2" || charset == "latin_2") {
    charset = "latin2";
  } else if (charset == "iso-2022") {
    charset = "iso-2022-jp";
  } else if (charset == "x-sjis" || charset == "shit_jis" ||
             charset == "x_sjis" || charset == "windows-31j" ||
             charset == "shift_js" || charset == "x-sjis-jp") {
    charset = "shift_jis";
  } else if (charset == "x-mac-roman") {
    charset = "macromania";
  } else if (charset == "iso-8869-15") {
    charset = "iso-8859-15";
  } else if (charset == "gb2313") {
    charset = "gb_2312-80";
  } else if (charset == "x-mac-turkish") {
    charset = "macturkish";
  } else if (charset == "euc_jp" || charset == "x-euc-jp" ||
             charset == "x-euc") {
    charset = "euc-jp";
  } else if (charset == "koi-8r") {
    charset = "koi8-r";
  } else if (charset == "iso-8859-8-i") {
    charset = "iso-8859-8";
  } else if (charset == "ksc5601" || charset == "euc_kr" ||
             charset == "ecu-kr") {
    charset = "korean";
  } else if (charset.find("windows") != std::string::npos ||
             charset == "widows-1252" || charset == "1252" ||
             charset == "windows" || charset == "windows_1258" ||
             charset == "windows-1046" || charset == "windows-1215" ||
             charset == "ansi_x3" || charset == "win" ||
             charset == "window-874" || charset == "ocidentalwindows-1252" ||
             charset == "ansi" || charset == "ishttp") {
    charset = "windows-1252";
  } else if (charset == "x-x-big5") {
    charset = "big5";
  }
  ConvertToUtf8(charset, document);
  return true;
}

static bool IsUndesiredHref(const std::string& href) {
  return href.empty() || href.at(0) == '#' || href.find("javascript") == 0 ||
         href.find("mailto") == 0;
}

// Extract only the text from the tree rooted at |node|.
// This method makes this library is Gumbo dependent.
static void ExtractText(GumboNode* node, std::string& text) {
  if (node->type == GUMBO_NODE_TEXT) {
    if (std::string(node->v.text.text).empty()) {
      text += " ";
    } else {
      text += " ";
      text += node->v.text.text;
      text += " ";
    }
    return;
  } else if (node->type == GUMBO_NODE_ELEMENT &&
             node->v.element.tag != GUMBO_TAG_SCRIPT &&
             node->v.element.tag != GUMBO_TAG_NOSCRIPT &&
             node->v.element.tag != GUMBO_TAG_STYLE) {
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
      GumboNode* child = (GumboNode*)children->data[i];
      ExtractText(child, text);
      if (child->type == GUMBO_NODE_ELEMENT &&
          child->v.element.tag == GUMBO_TAG_META) {
        if (gumbo_get_attribute(&child->v.element.attributes, "content") !=
            nullptr) {
          std::string str =
              gumbo_get_attribute(&child->v.element.attributes, "content")
                  ->value;
          if (str.find("text/html") == std::string::npos &&
              str.find("charset=") == std::string::npos) {
            text.append(" ");
            text.append(str);
            text.append(" ");
          }
        }
      }
    }
  }
}

static void ExtractAnchorInformation(
    GumboNode* node,
    std::vector<std::pair<std::string, std::string>>& referred_pages) {
  if (node->type != GUMBO_NODE_ELEMENT) {
    return;
  }
  GumboAttribute* href;
  if (node->v.element.tag == GUMBO_TAG_A &&
      (href = gumbo_get_attribute(&node->v.element.attributes, "href")) &&
      !IsUndesiredHref(href->value)) {
    std::string text = "";
    ExtractText(node, text);
    TreatText(text);
    referred_pages.push_back(std::make_pair(href->value, text));
  }

  GumboVector* children = &node->v.element.children;
  for (unsigned int i = 0; i < children->length; ++i) {
    ExtractAnchorInformation(static_cast<GumboNode*>(children->data[i]),
                             referred_pages);
  }
}

static unsigned PageId(
    const std::string& url,
    const std::unordered_map<std::string, PageInformation>& known_pages) {
  unsigned page_id = known_pages.size();
  ::util::PageUrl aux_url(url);
  if (known_pages.count(aux_url.url())) {
    page_id = known_pages.at(aux_url.url()).id;
  }
  return page_id;
}

std::unique_ptr<::util::Page> Parser::Parse(
    const std::string& url, const std::string& converted_document,
    std::vector<std::pair<std::string, std::string>>& referred_pages) {
  unsigned page_id = PageId(url, page_knowledge_->known_pages());
  std::unique_ptr<::util::Page> page(new ::util::Page(page_id, url, ""));
  page_knowledge_->known_pages()[page->url()].already_read = true;

  GumboOutput* output = gumbo_parse(converted_document.c_str());

  // Get only the text from the document.
  std::string& text = page->mutable_text();

  ExtractText(output->root, text);

  // Now we have to extract anchor text data.
  referred_pages.clear();
  ExtractAnchorInformation(output->root, referred_pages);

  // Lets free the structure since we don't need it anymore.
  gumbo_destroy_output(&kGumboDefaultOptions, output);

  // Remove all accents.
  TreatText(text);

  return page;
}

bool Parser::GenerateRankingData(
    const std::unique_ptr<::util::Page>& page,
    const std::vector<std::pair<std::string, std::string>>& referred_pages) {
  if (!GenerateTuples(page, false)) {
    return false;
  }
  if (!GenerateLinkData(page, referred_pages)) {
    return false;
  }

  ranking_metadata_->WritePage(page);

  return true;
}

bool Parser::GenerateTuples(const std::unique_ptr<::util::Page>& page,
                            bool is_anchor_page) {
  if (file_manager_ == nullptr || vocabulary_ == nullptr ||
      anchor_file_manager_ == nullptr) {
    return false;
  }
  std::string token = " ";
  unsigned search_position = 0, document_position = 0;
  if (is_anchor_page) {
    document_position =
        page_knowledge_->known_pages()[page->url()].anchor_position;
  }
  std::vector<std::unique_ptr<::util::Tuple>> tuples;
  std::unordered_map<unsigned, unsigned> frequencies;

  while (search_position < page->text().size()) {
    token = page->GetNextTokenFromText(search_position);

    // Stop word hash <<<< vocabulary hash -> much faster lookup.
    if (!token.empty() && !vocabulary_->CheckStopWords(token)) {
      unsigned term_id = 0;
      if (!vocabulary_->Check(token)) {
        term_id = vocabulary_->Insert(token).first;
      } else {
        term_id = vocabulary_->GetMappedValue(token).first;
      }
      frequencies[term_id]++;
      std::unique_ptr<::util::Tuple> t(
          new ::util::Tuple(term_id, page->page_id(), 0, document_position));
      document_position++;
      tuples.push_back(std::move(t));
    }
  }
  if (is_anchor_page) {
    page_knowledge_->known_pages()[page->url()].anchor_position =
        document_position;
  }

  auto t = tuples.begin();
  while (t != tuples.end()) {
    (*t)->frequency = frequencies[(*t)->term];
    // If the page is part of an anchor text, we use the appropriate manager.
    if (is_anchor_page) {
      anchor_file_manager_->WriteTuple(t->get());
    } else {
      file_manager_->WriteTuple(t->get());
    }
    ++t;
  }
  return true;
}

bool Parser::GenerateLinkData(
    const std::unique_ptr<::util::Page>& page,
    const std::vector<std::pair<std::string, std::string>>& referred_pages) {
  unsigned i = 0;
  while (i < referred_pages.size()) {
    std::string final_url = referred_pages[i].first, original_url = final_url;
    // If the href is a relative page, we gotta make it into a absolute one.
    if (::util::PageUrl::IsRelative(final_url)) {
      ::util::PageUrl current_url(page->url());
      current_url.RelativeMove(final_url);
      final_url = current_url.url();
    } else {
      ::util::PageUrl current_url(final_url);
      final_url = current_url.url();
    }
    if (page_knowledge_->known_pages().count(final_url)) {
      unsigned page_id = PageId(final_url, page_knowledge_->known_pages());
      std::unique_ptr<::util::Page> anchor_page(
          new ::util::Page(page_id, final_url, referred_pages[i].second));
      if (!GenerateTuples(anchor_page, true)) {
        return false;
      }
      if (page->page_id() != page_id) {
        page->UpdateLinks(page_id, false);
      }
    }
    ++i;
  }
  return true;
}

void Parser::DumpVocabulary(const std::string& file_path) {
  vocabulary_->Dump(file_path);
}

bool Parser::AlreadyRead(const std::string& page_url) {
  bool ret_val = page_knowledge_->AlreadyRead(page_url);

  return ret_val;
}

}  // namespace parsing
