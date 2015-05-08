#include "page_url.h"

namespace util {

PageUrl::PageUrl(const std::string& url) {
  is_absolute_ = 0;
  url_ = url;
  TokenizeUrl();
}

// Returns true if |url| has any of the prefixes' below. Otherwise, false.
static bool RemovePrefix(std::string& url) {
  std::vector<std::string> prefixes = { "http://www.", "https://www.",
                                        "http://ww.",  "https://ww.",
                                        "http://",  "https://",
                                        "www.",        "ww.",
                                        "//" };
  unsigned prefix = 0;
  while (prefix < prefixes.size()) {
    if (!url.find(prefixes[prefix])) {
      url.erase(0, prefixes[prefix].size());
      return true;
    }
    prefix++;
  }
  return false;
}

// Makes a copy of |url| up to and including the last "/", and stores it in
// |base_url|.
static void BaseUrl(const std::string& url, std::string& base_url) {
  unsigned pos = url.find("/");
  if (pos != std::string::npos) {
    base_url = url.substr(0, pos);
  }
  else {
    base_url.clear();
  }
}

static void AssembleUrl(std::vector<std::string>& tokens, std::string& url) {
  url.clear();
  unsigned i = 0;
  while(i < tokens.size()) {
    if(tokens[i].back() != '/') {
      tokens[i] += '/';
    }
    url += tokens[i];
    i++;
  } 
}

static void TokenizeUrlInternal(const std::string& url, std::vector<std::string>& tokens) {
  tokens.clear();
  unsigned i = 0, end_pos = url.size();

  end_pos = url.size();
  std::string token = "";

  // In case url is something like "/dir".
  if (!end_pos) {
    end_pos = url.size();
  }

  while (i < end_pos) {
    token += url[i];
    if (url[i] == '/' && !token.empty()) {
      tokens.push_back(token);
      token.clear();
    }
    i++;
  }

  // We have to push the effective html document to which |url_| is pointing to,
  // except if the base page is enough to reference it.
  if (!token.empty() && token.compare("index.html") != 0) {
    tokens.push_back(token);
  }
}

void PageUrl::TokenizeUrl() {
  if(url_.empty()) {
    return;
  }
  is_absolute_ = RemovePrefix(url_);
  TokenizeUrlInternal(url_, tokens_);
  AssembleUrl(tokens_, url_);
}

bool PageUrl::IsRelative(const std::string& url) {
  std::vector<std::string> prefixes = { "http://www.", "https://www.",
                                        "http://ww.",  "https://ww.",
                                        "www.",        "ww.",
                                        "//" };
  unsigned prefix = 0;
  while (prefix < prefixes.size()) {
    if (!url.find(prefixes[prefix])) {
      return false;
    }
    prefix++;
  }
  return true;
}

PageUrl::MoveAction GetAction(const std::string& token) {
  if(token == "./") {
    return PageUrl::MoveAction::STAY;
  }
  if(token == "../") {
    return PageUrl::MoveAction::MOVE_DOWN;
  }
  if(token == "/") {
    return PageUrl::MoveAction::MOVE_TO_ROOT;
  }
  return PageUrl::MOVE_UP;
}

void PageUrl::RelativeMove(const std::string& href) {
  if(!IsRelative(href)) {
    return;
  }
  // Get the associated base url.
  std::string base_url;
  BaseUrl(url_, base_url);

  // Tokenize |href|.
  std::vector<std::string> href_tokens;
  TokenizeUrlInternal(href, href_tokens);
  unsigned i = 0;
  while(i < href_tokens.size()) {
    MoveAction action = GetAction(href_tokens[i]);
    if(action == MoveAction::STAY) {
    }
    else if(action == MoveAction::MOVE_DOWN) {
      if(tokens_.size() > 1) {
        tokens_.pop_back();
      }
    }
    else if(action == MoveAction::MOVE_TO_ROOT) {
      tokens_.clear();
      tokens_.push_back(base_url);
    }
    else if(action == MoveAction::MOVE_UP) {
      tokens_.push_back(href_tokens[i]);
    }
    i++;
  }
  AssembleUrl(tokens_, url_);
}

}  // namespace util
