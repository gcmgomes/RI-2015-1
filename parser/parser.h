#ifndef _RI_2015_1_PARSER_PARSER_H_
#define _RI_2015_1_PARSER_PARSER_H_

#include <iostream>
#include <memory>
#include "page_knowledge.h"
#include "vocabulary.h"
#include "../ranking/ranking_metadata.h"
#include "../util/page.h"
#include "../util/file_manager.h"

namespace parsing {

class Parser {
 public:
  // Takes ownership of every pointer.
  Parser(::util::FileManager* file_manager,
         ::util::FileManager* anchor_file_manager, Vocabulary* vocabulary,
         PageKnowledge* page_knowledge,
         ranking::RankingMetadata* ranking_metadata)
      : file_manager_(file_manager), anchor_file_manager_(anchor_file_manager),
        vocabulary_(vocabulary), page_knowledge_(page_knowledge),
        ranking_metadata_(ranking_metadata){};

  ~Parser(){};

  // Converts |document| to a parsable format. Returns true if successful,
  // false otherwise.
  bool Convert(std::string& document);

  // Parses a previously converted |parsable_document| and returns an object
  // containing its data. |referred_pages| holds all hyperlink and ancchor text
  // information.
  std::unique_ptr<::util::Page> Parse(
      const std::string& url, const std::string& parsable_document,
      std::vector<std::pair<std::string, std::string>>& referred_pages);

  // Generates any data used for ranking purposes and process it with
  // |ranking_metadata_|.
  bool GenerateRankingData(
      const std::unique_ptr<::util::Page>& page,
      const std::vector<std::pair<std::string, std::string>>& referred_pages);

  // Generates tuples contained within |page| with |document_id| as one of their
  // fields and outputs them to |file_manager_|'s output file. If
  // |is_anchor_page| == true, use |anchor_file_manager_| instead.
  // Returns false if the tuples cannot be generated, true otherwise.
  bool GenerateTuples(const std::unique_ptr<::util::Page>& page,
                      bool is_anchor_page);

  // Handles link data information generation supplied by |referred_pages|.
  // Outlink data is added to |page|.
  bool GenerateLinkData(
      const std::unique_ptr<::util::Page>& page,
      const std::vector<std::pair<std::string, std::string>>& referred_pages);

  void DumpVocabulary(const std::string& file_path);

 private:
  std::unique_ptr<::util::FileManager> file_manager_;

  // File manager for anchor text.
  std::unique_ptr<::util::FileManager> anchor_file_manager_;

  std::unique_ptr<Vocabulary> vocabulary_;

  // Mapping url -> unsigned. The key should be normalized according to
  // ::util::PageUrl.
  std::unique_ptr<PageKnowledge> page_knowledge_;

  std::unique_ptr<::ranking::RankingMetadata> ranking_metadata_;
};

}  // namespace parsing

#endif
