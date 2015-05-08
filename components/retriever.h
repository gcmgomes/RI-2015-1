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

  const std::unique_ptr<::parsing::Vocabulary>& vocabulary() {
    return vocabulary_;
  }

  // Properly initializes |vocabulary_|
  void Init(const std::string& vocabulary_file_path);

  // Retrieve all the documents associated with |query|. A |query| is a sequence
  // of terms without any logical operators.
  void Retrieve(const std::string& query, std::vector<::util::Page>& answers);

  // Utility to get the tokens out of a |query|.
  static void GetToken(const std::string& query, unsigned& pos,
                       std::string& token);

  // Recover the index entry associated with |term| and stores it at |entry|.
  bool GetIndexEntry(const std::string& term, IndexEntry& entry,
                     bool anchor_index);

 private:
  // Convert the values in |doc_set| in meaningful Page objects stored in
  // |page_set|.
  void ConvertIntoPages(const std::unordered_set<unsigned>& doc_set,
                        std::vector<::util::Page>& page_set);

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
