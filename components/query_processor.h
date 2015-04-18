#ifndef _RI_2015_1_COMPONENTS_QUERY_PROCESSOR_H_
#define _RI_2015_1_COMPONENTS_QUERY_PROCESSOR_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include "indexer.h"
#include "index_entry.h"
#include "../parser/vocabulary.h"

namespace components {

class QueryProcessor {
 public:
  QueryProcessor(const std::string& index_file_path);

  // Contructs the temporary bridge between the index file and |vocabulary_|.
  void BuildBridge(const std::string& vocabulary_path);

  // Process |query| and returns all the information about the documents that
  // satisfy it.
  std::set<unsigned> QueryIndex(const std::string& query);

 private:
  // Gets the |key|'s entry in the index file and stores the results in |entry|.
  // Returns true if successful, false otherwise. Must check the result before
  // inspecting |entry|.
  bool GetIndexEntry(const std::string& key, IndexEntry& entry);

  std::unordered_map<unsigned, unsigned > bridge_;
  std::unique_ptr<parsing::Vocabulary> vocabulary_;
  std::unique_ptr<std::fstream> index_file_;
};

}  // namespace components

#endif
