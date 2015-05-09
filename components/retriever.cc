#include <map>
#include <unordered_map>
#include <vector>
#include "retriever.h"
#include "indexer.h"

namespace components {

Retriever::Retriever(const std::string& index_file_path,
                     const std::string& anchor_index_file_path,
                     const std::string& ranking_metadata_file_path) {
  index_file_.reset(new std::fstream());
  index_file_->open(index_file_path.c_str(),
                    std::fstream::in | std::fstream::binary);

  anchor_index_file_.reset(new std::fstream());
  anchor_index_file_->open(anchor_index_file_path.c_str(),
                           std::fstream::in | std::fstream::binary);
  ranking_metadata_.reset(
      new ::ranking::RankingMetadata(ranking_metadata_file_path));
}

static void FillBridge(
    std::unique_ptr<std::fstream>& index_file,
    std::unique_ptr<::ranking::RankingMetadata>& ranking_metadata,
    std::unordered_map<unsigned, ::parsing::VocabularyEntry>& bridge,
    bool anchor_bridge, bool init_weights) {
  // Passes through the index once to get information on every term.
  while (!index_file->eof() && index_file->peek() != EOF) {
    unsigned position_in_file = index_file->tellg();

    IndexEntry entry;
    if (!Indexer::GetNextEntry(index_file, entry)) {
      break;
    }

    if (init_weights) {
      // We have to put |position_in_file| instead of the |term_id| due to the
      // vocabulary overwrite.
      ranking_metadata->UpdatePagesWeights(position_in_file,
                                           entry.occurrences(), anchor_bridge);
    }

    if (!anchor_bridge) {
      bridge[entry.term()].first = position_in_file;
    } else {
      bridge[entry.term()].second = position_in_file;
    }
  }

  index_file->seekg(0, index_file->beg);
}

void Retriever::Init(const std::string& vocabulary_file_path, bool init_weights) {
  ranking_metadata_->LoadPages();
  std::unordered_map<unsigned, ::parsing::VocabularyEntry> bridge;
  FillBridge(index_file_, ranking_metadata_, bridge, false, init_weights);
  FillBridge(anchor_index_file_, ranking_metadata_, bridge, true, init_weights);

  // Initialize the vocabulary with just enough buckets to fit every term.
  vocabulary_.reset(new parsing::Vocabulary(bridge.size() + 1));
  vocabulary_->Load(vocabulary_file_path, bridge);

  ranking_metadata_->CalculatePagesLengths();
  ranking_metadata_->FixInlinks();
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

void Retriever::UpdatePagesToFile() {
  ranking_metadata_->UpdatePagesToFile();
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

static void MergeEntry(IndexEntry& anchor_entry, IndexEntry& real_entry) {
  auto current = anchor_entry.mutable_occurrences().begin();
  while (current != anchor_entry.occurrences().end()) {
    if (!real_entry.occurrences().count(current->first)) {
      real_entry.mutable_occurrences()[current->first] = current->second;
    }
    ++current;
  }
}

void Retriever::Retrieve(const std::string& query,
                         std::vector<::util::Page>& answers) {
  bool use_anchor_index = false;
  answers.clear();
  unsigned pos = 0, min_pos = 0, min_size = 0;
  IndexEntry entry, anchor_entry;
  std::vector<std::string> tokens;
  while (pos < query.size()) {
    std::string token = "";
    GetToken(query, pos, token);

    if (token.empty() ||
        (!GetIndexEntry(token, entry, use_anchor_index) &&
         !GetIndexEntry(token, anchor_entry, !use_anchor_index))) {
      continue;
    }
    tokens.push_back(token);
    // Update the minimum conjuntive set size needed.
    unsigned current_size =
        entry.occurrences().size() + anchor_entry.occurrences().size();
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
  GetIndexEntry(tokens[min_pos], entry, use_anchor_index);
  ExtractDocuments(entry, small_set);
  std::unordered_map<unsigned, IndexEntry> entries;
  while (pos < tokens.size()) {
    if (pos != min_pos) {
      std::unordered_set<unsigned> current;
      GetIndexEntry(tokens[pos], entry, use_anchor_index);
      GetIndexEntry(tokens[pos], anchor_entry, !use_anchor_index);
      MergeEntry(entry, anchor_entry);
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

bool Retriever::GetIndexEntry(const std::string& term, IndexEntry& entry,
                              bool anchor_index) {
  if (!vocabulary_->Check(term)) {
    return false;
  }
  ::parsing::VocabularyEntry index_position = vocabulary_->GetMappedValue(term);
  if (!anchor_index) {
    index_file_->seekg(index_position.first);
    components::Indexer::GetNextEntry(index_file_, entry);
  } else {
    anchor_index_file_->seekg(index_position.second);
    components::Indexer::GetNextEntry(anchor_index_file_, entry);
  }
  return true;
}

}  // namespace components
