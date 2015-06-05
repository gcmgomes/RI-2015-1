#ifndef _RI_2015_1_COMPONENTS_RETRIEVER_H_
#define _RI_2015_1_COMPONENTS_RETRIEVER_H_

#include <iostream>
#include <sstream>
#include <map>
#include <unordered_set>
#include "index_entry.h"
#include "../parser/vocabulary.h"
#include "../ranking/ranking_metadata.h"
#include "../util/page.h"

namespace components {

class Retriever {
 public:
  Retriever(const std::string& index_file_path,
            const std::string& anchor_index_file_path,
            const std::string& ranking_metadata_file_path);

  const std::unique_ptr<::parsing::Vocabulary>& vocabulary() const {
    return vocabulary_;
  }

  // Properly initializes |vocabulary_|. If |remap_entries| is true, replace the
  // entries in |vocabulary| by their positions in the indexes.
  void Init(const std::string& vocabulary_file_path, bool remap_entries = false);

  // Retrieve all the documents associated with |query|. A |query| is a sequence
  // of terms without any logical operators. The resulting page ids are stored
  // in |answers|
  void Retrieve(const std::string& query,
                std::unordered_set<unsigned>& answers);

  // Utility to get the tokens out of a |query|.
  static void GetToken(const std::string& query, unsigned& pos,
                       std::string& token);

  // Recover the index entry associated with |term| and stores it at |entry|.
  bool GetIndexEntry(const std::string& term, IndexEntry& entry,
                     bool anchor_index);

  // Returns the pair (length, anchor_length) of the page object indexed by
  // |page_id|.
  std::pair<double, double> PageLengths(unsigned page_id);

  // Returns the amount of pages present in the collection.
  unsigned PageCount() const;

  // Loads the ::util::Page object indexed by |page_id|.
  std::unique_ptr<::util::Page> LoadPage(unsigned page_id);

  // Extracts the id and url from the pages mapped by |scored_answers| and
  // stores them in |scored_pages|.
  void ExtractAnswerPages(
      const std::unordered_map<unsigned, double>& scored_answers,
      std::vector<::util::Page>& scored_pages);

 private:
  // The mapping needed to retrieve documents from the index.
  std::unique_ptr<::parsing::Vocabulary> vocabulary_;

  // Metadata associated with the collections' documents;
  std::unique_ptr<::ranking::RankingMetadata> ranking_metadata_;

  // The file where the index is stored.
  std::unique_ptr<std::fstream> index_file_;

  // The file where the anchor text index is stored;
  std::unique_ptr<std::fstream> anchor_index_file_;
};

}  // namespace components

#endif
