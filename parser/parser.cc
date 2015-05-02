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

namespace parsing {

// Remove useless information present in |source|'s header.
static void RemoveHeader(std::string& source) {
  unsigned pos = source.find('<');
  if (pos && pos != std::string::npos) {
    source.erase(0, pos - 1);
  }
}

void Parser::Convert(std::string& document) {
  RemoveHeader(document);
  ConvertToUtf8(document);
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

std::unique_ptr<::util::Page> Parser::Parse(unsigned page_id,
    const std::string& url, const std::string& converted_document) {
  GumboOutput* output = gumbo_parse(converted_document.c_str());

  // Get only the text from the document.
  std::string text = "";
  text = ExtractText(output->root);

  // Remove all accents.
  TreatText(text);

  std::unique_ptr<::util::Page> page(new ::util::Page(page_id, url, text));

  gumbo_destroy_output(&kGumboDefaultOptions, output);

  return page;
}

bool Parser::GenerateRankingData(const std::unique_ptr<::util::Page>& page) {
  if (!GenerateTuples(page)) {
    return false;
  }

  ranking_metadata_->WritePage(page);
  return true;
}

bool Parser::GenerateTuples(const std::unique_ptr<::util::Page>& page) {
  if (file_manager_ == nullptr || vocabulary_ == nullptr) {
    return false;
  }
  std::string token = " ";
  unsigned search_position = 0, document_position = 0;
  std::vector<std::unique_ptr<::util::Tuple>> tuples;
  std::unordered_map<unsigned, unsigned> frequencies;

  while (!token.empty()) {
    token = page->GetNextTokenFromText(search_position);

    // Stop word hash <<<< vocabulary hash -> much faster lookup.
    if (!token.empty() && !vocabulary_->CheckStopWords(token)) {
      unsigned term_id = 0;
      if (!vocabulary_->Check(token)) {
        term_id = vocabulary_->Insert(token);
      } else {
        term_id = vocabulary_->GetMappedValue(token);
      }

      frequencies[term_id]++;
      std::unique_ptr<::util::Tuple> t(
          new ::util::Tuple(term_id, page->page_id(), 0, document_position++));
      tuples.push_back(std::move(t));
    }
  }
  auto t = tuples.begin();
  while (t != tuples.end()) {
    (*t)->frequency = frequencies[(*t)->term];
    file_manager_->WriteTuple(t->get());
    ++t;
  }
  return true;
}

void Parser::DumpVocabulary(const std::string& file_path) {
  vocabulary_->Dump(file_path);
}

}  // namespace parsing
