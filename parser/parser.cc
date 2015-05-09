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
  ConvertToUtf8(document);
  return true;
}

static bool IsUndesiredHref(const std::string& href) {
  return href.at(0) == '#' || href.find("javascript") == 0 ||
         href.find("mailto") == 0;
}

// Extract only the text from the tree rooted at |node|.
// This method makes this library is Gumbo dependent.
static std::string ExtractText(GumboNode* node) {
  if (node->type == GUMBO_NODE_TEXT) {
    return std::string(node->v.text.text);
  } else if (node->type == GUMBO_NODE_ELEMENT &&
             node->v.element.tag != GUMBO_TAG_SCRIPT &&
             node->v.element.tag != GUMBO_TAG_NOSCRIPT &&
             node->v.element.tag != GUMBO_TAG_STYLE) {
    std::string contents = "";
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
      const std::string text = ExtractText((GumboNode*)children->data[i]);
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
    std::string text = ExtractText(node);
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
  GumboOutput* output = gumbo_parse(converted_document.c_str());

  // Get only the text from the document.
  std::string text = "";
  text = ExtractText(output->root);

  // Remove all accents.
  TreatText(text);

  unsigned page_id = PageId(url, page_knowledge_->known_pages());

  std::unique_ptr<::util::Page> page(new ::util::Page(page_id, url, text));

  // Now we have to extract anchor text data.
  referred_pages.clear();
  ExtractAnchorInformation(output->root, referred_pages);

  gumbo_destroy_output(&kGumboDefaultOptions, output);

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

  while (!token.empty()) {
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
          new ::util::Tuple(term_id, page->page_id(), 0, document_position++));
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
      page->UpdateLinks(page_id, false);
    }
    ++i;
  }
  return true;
}

void Parser::DumpVocabulary(const std::string& file_path) {
  vocabulary_->Dump(file_path);
}

}  // namespace parsing
