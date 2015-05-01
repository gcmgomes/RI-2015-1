#ifndef _RI_2015_1_PARSER_PARSER_H_
#define _RI_2015_1_PARSER_PARSER_H_

#include <iostream>
#include <memory>
#include "vocabulary.h"
#include "../ranking/ranking_metadata.h"
#include "../util/page.h"
#include "../util/file_manager.h"

namespace parsing {

class Parser {
 public:
  // Takes ownership of every pointer.
  Parser(::util::FileManager* file_manager, Vocabulary* vocabulary,
         ranking::RankingMetadata* ranking_metadata)
      : file_manager_(file_manager), vocabulary_(vocabulary),
        ranking_metadata_(ranking_metadata){};

  Parser() : Parser(nullptr, nullptr, nullptr){};

  Parser(::util::FileManager* file_manager)
      : Parser(file_manager, nullptr, nullptr){};

  Parser(Vocabulary* vocabulary) : Parser(nullptr, vocabulary, nullptr){};

  Parser(::ranking::RankingMetadata* ranking_metadata)
      : Parser(nullptr, nullptr, ranking_metadata){};

  ~Parser(){};

  // Converts |document| to a parsable format.
  void Convert(std::string& document);

  // Parses a previously converted |parsable_document| and returns an object
  // containing its data.
  std::unique_ptr<::util::Page> Parse(unsigned page_id, const std::string& url,
                                      const std::string& parsable_document);

  // Generates any data used for ranking purposes and process it with
  // |ranking_metadata_|.
  bool GenerateRankingData(const std::unique_ptr<::util::Page>& page);

  // Generates tuples contained within |page| with |document_id| as one of their
  // fields and outputs them to |file_manager_|'s output file.
  // Returns false if the tuples cannot be generated, true otherwise.
  bool GenerateTuples(const std::unique_ptr<::util::Page>& page);

  void DumpVocabulary(const std::string& file_path);

 private:
  std::unique_ptr<::util::FileManager> file_manager_;

  std::unique_ptr<Vocabulary> vocabulary_;

  std::unique_ptr<::ranking::RankingMetadata> ranking_metadata_;
};

}  // namespace parsing

#endif
