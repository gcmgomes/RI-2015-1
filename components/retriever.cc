#include <map>
#include <unordered_map>
#include <vector>
#include "retriever.h"
#include "indexer.h"

namespace components {

Retriever::Retriever(const std::string& index_file_path,
                     const std::string& ranking_metadata_file_path) {
  index_file_.reset(new std::fstream());
  index_file_->open(index_file_path.c_str(),
                    std::fstream::in | std::fstream::binary);
  ranking_metadata_.reset(
      new ::ranking::RankingMetadata(ranking_metadata_file_path));
}

void Retriever::Init(const std::string& vocabulary_file_path) {
  ranking_metadata_->LoadPages();
  std::unordered_map<unsigned, unsigned> bridge;
  // Passes through the index once to get information on every term.
  while (!index_file_->eof() && index_file_->peek() != EOF) {
    unsigned position_in_file = index_file_->tellg();

    IndexEntry entry;
    if (!Indexer::GetNextEntry(index_file_, entry)) {
      break;
    }

    // We have to put |position_in_file| instead of the |term_id| due to the vocabulary overwrite.
    ranking_metadata_->UpdatePagesWeights(position_in_file, entry.occurrences());
    bridge[entry.term()] = position_in_file;
  }
  index_file_->seekg(0, index_file_->beg);
  // Initialize the vocabulary with just enough buckets to fit every term.
  vocabulary_.reset(new parsing::Vocabulary(bridge.size() + 1));
  vocabulary_->Load(vocabulary_file_path, bridge);

  ranking_metadata_->CalculatePagesLengths();
}

void Retriever::GetToken(const std::string& query, unsigned& pos,
                         std::string& token) {
  token.clear();
  while (pos < query.size() && std::isalnum(query[pos])) {
    token += query[pos];
    pos++;
  }
  pos++;
}

static void ExtractDocuments(const IndexEntry& entry,
                             std::unordered_set<unsigned>& doc_set) {
  doc_set.clear();
  const auto& mappings = entry.occurrences();
  auto i = mappings.begin();
  while (i != mappings.end()) {
    doc_set.insert(i->first);
    ++i;
  }
}

static void Intersect(std::unordered_set<unsigned>& small_set,
                      const std::unordered_set<unsigned>& large_set) {
  auto copy = small_set;
  auto i = copy.begin();
  while (i != copy.end()) {
    if (!large_set.count(*i)) {
      small_set.erase(*i);
    }
    ++i;
  }
}

void Retriever::Retrieve(const std::string& query,
                         std::vector<::util::Page>& answers) {
  answers.clear();
  unsigned pos = 0, min_pos = 0, min_size = 0;
  IndexEntry entry;
  std::vector<std::string> tokens;
  while (pos < query.size()) {
    std::string token = "";
    GetToken(query, pos, token);

    if (token.empty() || !GetIndexEntry(token, entry)) {
      continue;
    }
    tokens.push_back(token);
    // Update the minimum conjuntive set size needed.
    unsigned current_size = entry.occurrences().size();
    if (min_size == 0) {
      min_size = current_size;
      min_pos = 0;
    } else if (current_size < min_size) {
      min_pos = tokens.size() - 1;
      min_size = current_size;
    }
  }
  if (tokens.empty()) {
    return;
  }
  pos = 0;
  std::unordered_set<unsigned> small_set;
  GetIndexEntry(tokens[min_pos], entry);
  ExtractDocuments(entry, small_set);
  std::unordered_map<unsigned, IndexEntry> entries;
  while (pos < tokens.size()) {
    if (pos != min_pos) {
      std::unordered_set<unsigned> current;
      GetIndexEntry(tokens[pos], entry);
      ExtractDocuments(entry, current);
      Intersect(small_set, current);
      entries[entry.term()] = entry;
    }
    pos++;
  }
  ConvertIntoPages(small_set, answers);
}

void Retriever::ConvertIntoPages(const std::unordered_set<unsigned>& doc_set,
                                 std::vector<::util::Page>& page_set) {
  auto& pages = ranking_metadata_->mutable_pages();
  auto i = doc_set.begin();
  while (i != doc_set.end()) {
    page_set.push_back(pages[*i]);
    ++i;
  }
}

bool Retriever::GetIndexEntry(const std::string& term, IndexEntry& entry) {
  if (!vocabulary_->Check(term)) {
    return false;
  }
  unsigned index_position = vocabulary_->GetMappedValue(term);
  index_file_->seekg(index_position);
  components::Indexer::GetNextEntry(index_file_, entry);
  return true;
}

}  // namespace components
