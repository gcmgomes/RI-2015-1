#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cctype>
#include "query_processor.h"

namespace components {

QueryProcessor::QueryProcessor(const std::string& index_file_path) {
  index_file_.reset(new std::fstream());
  index_file_->open(index_file_path.c_str(),
                    std::fstream::in | std::fstream::binary);
}

void QueryProcessor::BuildBridge(const std::string& vocabulary_path) {
  // Passes through the index once to get information on every term.
  while (!index_file_->eof() && index_file_->peek() != EOF) {
    unsigned position_in_file = index_file_->tellg();

    IndexEntry entry;
    Indexer::GetNextEntry(index_file_, entry);

    bridge_[entry.term()] = position_in_file;
  }
  index_file_->seekg(0, index_file_->beg);
  // Initialize the vocabulary with just enough buckets to fit every term.
  vocabulary_.reset(new parsing::Vocabulary(bridge_.size()));
  vocabulary_->LoadTerms(vocabulary_path, bridge_);
  bridge_.clear();
}

// Returns true if |q| has an AND beginning at |position|.
static bool IsAnd(const std::string& q, unsigned position) {
  if (position + 2 >= q.size()) {
    return false;
  }
  return q[position] == 'A' && q[position + 1] == 'N' && q[position + 2] == 'D';
}

// Returns true if |q| has an OR beginning at |position|.
static bool IsOr(const std::string& q, unsigned position) {
  if (position + 1 >= q.size()) {
    return false;
  }
  return q[position] == 'O' && q[position + 1] == 'R';
}

// Nices up |query| so parsing it becomes even easier.
static std::string TreatQuery(const std::string& query) {
  std::string str = "";
  unsigned i = 0;
  while (i < query.size()) {
    if (query[i] != ' ') {
      if (IsAnd(query, i)) {
        i += 2;
        str += '*';
      } else if (IsOr(query, i)) {
        i++;
        str += '+';
      } else {
        str += query[i];
      }
    }
    i++;
  }
  return str;
}

static void ExtractSet(IndexEntry& entry, std::set<unsigned>& docs) {
  docs.clear();
  auto i = entry.occurrences().begin();
  while (i != entry.occurrences().end()) {
    docs.insert(i->first);
    ++i;
  }
}

static void Intersect(std::set<unsigned>& answer, std::set<unsigned>& other) {
  std::set<unsigned> real_answer;
  std::set_intersection(answer.begin(), answer.end(), other.begin(),
                        other.end(),
                        std::inserter(real_answer, real_answer.begin()));
  answer = real_answer;
}

static void Union(std::set<unsigned>& answer, const std::set<unsigned>& other) {
  std::set<unsigned> real_answer;
  std::set_union(answer.begin(), answer.end(), other.begin(), other.end(),
                 std::inserter(real_answer, real_answer.begin()));
  answer = real_answer;
}

static void GetToken(const std::string& parsable, unsigned& pos,
                     std::string& token) {
  token.clear();
  while (pos < parsable.size() && std::isalnum(parsable[pos])) {
    token += parsable[pos];
    pos++;
  }
}

std::set<unsigned> QueryProcessor::QueryIndex(const std::string& query) {
  std::string parsable = TreatQuery(query);
  unsigned i = 0;
  char operation = ' ';
  std::set<unsigned> answer, other;

  // Get the first word.
  std::string token = "";
  GetToken(parsable, i, token);
  // Get it's documents.
  IndexEntry entry;
  GetIndexEntry(token, entry);
  ExtractSet(entry, answer);
  while (i < parsable.size()) {
    operation = parsable[i];
    i++;
    GetToken(parsable, i, token);
    IndexEntry entry;
    GetIndexEntry(token, entry);
    ExtractSet(entry, other);
    // AND.
    if (operation == '*') {
      Intersect(answer, other);
    }
    // OR.
    else {
      Union(answer, other);
    }
  }
  return answer;
}

bool QueryProcessor::GetIndexEntry(const std::string& key, IndexEntry& entry) {
  if (!vocabulary_->CheckTerms(key)) {
    return false;
  }
  unsigned index_position = vocabulary_->GetMappedValue(key);
  index_file_->seekg(index_position);
  components::Indexer::GetNextEntry(index_file_, entry);
  return true;
}

}  // namespace components
