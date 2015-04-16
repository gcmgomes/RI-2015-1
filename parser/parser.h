#ifndef _RI_2015_1_PARSER_PARSER_H_
#define _RI_2015_1_PARSER_PARSER_H_

#include <iostream>
#include <memory>
#include "vocabulary.h"
#include "../util/page.h"
#include "../util/file_manager.h"

namespace parsing {

class Parser {
 public:
  // Takes ownership of |file_manager| and |vocabulary|.
  Parser(::util::FileManager* file_manager, Vocabulary* vocabulary)
      : file_manager_(file_manager), vocabulary_(vocabulary){};

  Parser() : Parser(nullptr, nullptr){};

  Parser(::util::FileManager* file_manager) : Parser(file_manager, nullptr){};

  Parser(Vocabulary* vocabulary) : Parser(nullptr, vocabulary){};

  ~Parser(){};

  // Converts |document| to a parsable format.
  void Convert(std::string& document);

  // Parses a previously converted |parsable_document| and returns an object
  // containing its data.
  std::unique_ptr<::util::Page> Parse(const std::string& url,
                                      const std::string& parsable_document);

  // Generates tuples contained within |page| with |documen_id| as one of their
  // fields and outputs them to |file_manager_|'s output file.
  // Returns false if the tuples cannot be generated, true otherwise.
  bool GenerateTuples(const std::unique_ptr<::util::Page>& page, unsigned document_id);

  void DumpVocabulary(const std::string& file_path);

 private:
  std::unique_ptr<::util::FileManager> file_manager_;

  std::unique_ptr<Vocabulary> vocabulary_;
};

}  // namespace parsing

#endif
