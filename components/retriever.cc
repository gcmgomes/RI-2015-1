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
    std::unordered_map<unsigned, ::parsing::VocabularyEntry>& bridge,
    bool anchor_bridge) {
  unsigned read_entries = 0;
  // Passes through the index once to get information on every term.
  while (!index_file->eof() && index_file->peek() != EOF) {
    unsigned position_in_file = index_file->tellg();

    IndexEntry entry;
    if (!Indexer::GetNextEntry(index_file, entry)) {
      break;
    }
    if (!(read_entries % 1000)) {
      std::cout << "\rRemapped entries " << read_entries;
      fflush(stdout);
    }
    read_entries++;
    if (!anchor_bridge) {
      bridge[entry.term()].first = position_in_file;
    } else {
      bridge[entry.term()].second = position_in_file;
    }
  }

  std::cout << "\rRemapped entries " << read_entries << std::endl;
  read_entries++;
  index_file->seekg(0, index_file->beg);
}

void Retriever::Init(const std::string& vocabulary_file_path,
                     bool remap_entries) {
  std::unordered_map<unsigned, ::parsing::VocabularyEntry> bridge;
  if (remap_entries) {
    FillBridge(index_file_, bridge, false);
    FillBridge(anchor_index_file_, bridge, true);
  } else {
    ranking_metadata_->LoadPages();
  }
  // Initialize the vocabulary with just enough buckets to fit every term.
  vocabulary_.reset(new parsing::Vocabulary(bridge.size() + 1));
  vocabulary_->Load(vocabulary_file_path, bridge);
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

static void Intersect(const std::unordered_set<unsigned>& large_set,
                      std::unordered_set<unsigned>& small_set) {
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
                         std::unordered_set<unsigned>& answers) {
  bool use_anchor_index = true;
  answers.clear();
  unsigned pos = 0, min_pos = 0, min_size = 0;
  IndexEntry entry, anchor_entry;
  std::vector<std::string> tokens;
  while (pos < query.size()) {
    std::string token = "";
    GetToken(query, pos, token);

    if (token.empty() ||
        (!GetIndexEntry(token, entry, !use_anchor_index) &&
         !GetIndexEntry(token, anchor_entry, use_anchor_index))) {
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
  GetIndexEntry(tokens[min_pos], entry, !use_anchor_index);
  ExtractDocuments(entry, small_set);
  while (pos < tokens.size()) {
    std::unordered_set<unsigned> current;
    GetIndexEntry(tokens[pos], entry, !use_anchor_index);
    GetIndexEntry(tokens[pos], anchor_entry, use_anchor_index);
    MergeEntry(anchor_entry, entry);
    ExtractDocuments(entry, current);
    Intersect(current, small_set);
    pos++;
  }
  answers = small_set;
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

std::pair<double, double> Retriever::PageLengths(unsigned page_id) {
  return ranking_metadata_->page_lengths().at(page_id);
}

unsigned Retriever::PageCount() const {
  return ranking_metadata_->pages().size();
}

std::unique_ptr<::util::Page> Retriever::LoadPage(unsigned page_id) {
  return ranking_metadata_->LoadPage(page_id);
}

void Retriever::ExtractAnswerPages(
    const std::unordered_map<unsigned, double>& scored_answers,
    std::vector<::util::Page>& scored_pages) {
  scored_pages.clear();
  auto page_id = scored_answers.begin();
  while (scored_answers.end() != page_id) {
    auto page = LoadPage(page_id->first);
    ::util::Page p(page_id->first, page->url(), page->title(), page->text(),
                   page->length(), page->anchor_length(), page->page_rank());
    p.mutable_score() = page_id->second;
    scored_pages.push_back(p);
    ++page_id;
  }
}

}  // namespace components
