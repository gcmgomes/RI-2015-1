#include "../parser.h"
#include "../vocabulary.h"
#include "../../ranking/ranking_metadata.h"
#include "../../util/page.h"
#include "../../util/file_manager.h"
#include "../ricp/src/Document.h"
#include "../ricp/src/CollectionReader.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 7) {
    std::cout << argv[0] << " [input directory] [index file] [stop words file] "
                            "[vocabulary dump file] [document metadata file "
                            "path] [tuple file path]" << endl;
    return 0;
  }

  std::fstream document_length_file;
  document_length_file.open(argv[5], std::fstream::binary | std::fstream::out);

  std::unique_ptr<parsing::Vocabulary> vocabulary(
      new parsing::Vocabulary(5000000));
  vocabulary->LoadStopWords(argv[3]);

  std::unique_ptr<ranking::RankingMetadata> ranking_metadata(
      new ranking::RankingMetadata(argv[5]));

  parsing::Parser parser(new ::util::FileManager(0, argv[6], argv[6]),
                         vocabulary.release(), ranking_metadata.release());

  RICPNS::CollectionReader* reader =
      new RICPNS::CollectionReader(argv[1], argv[2]);
  RICPNS::Document doc;

  unsigned total_documents = 0;
  while (reader->getNextDocument(doc)) {
    std::string contents = doc.getText(), url = doc.getURL();
    doc.clear();
    parser.Convert(contents);
    std::unique_ptr<util::Page> page =
        std::move(parser.Parse(total_documents, url, contents));
    if (page->text().empty()) {
      continue;
    }
    if (!parser.GenerateRankingData(page)) {
      cout << endl;
      std::cerr << "Something massively wrong happened, aborting" << std::endl;
      return -1;
    }
    if (total_documents % 1000 == 0) {
      cout << "\rProcessed Documents: " << total_documents;
      fflush(stdout);
    }
    total_documents++;
  }
  cout << "\rProcessed Documents: " << total_documents;
  fflush(stdout);
  cout << endl;
  parser.DumpVocabulary(argv[4]);
  delete reader;
}
