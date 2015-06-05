#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include "../page_knowledge.h"
#include "../parser.h"
#include "../vocabulary.h"
#include "../../ranking/ranking_metadata.h"
#include "../../util/page.h"
#include "../../util/file_manager.h"
#include "../ricp/src/Document.h"
#include "../ricp/src/CollectionReader.h"

using namespace std;

bool stuff(string url) {
  return url == "http://cartas.com.br/" ||
         url == "http://www.juegarcasinos.com/pai-gow-poquer-internet.php" ||
         url == "http://www.baixaki.com.br/jogos/" ||
         url == "http://clubedepoquer.com/" ||
         url == "http://www.mypoquerenlinea.com/17099.html" ||
         url == "http://www.mypoquers.com/holdem-poquers-online.html" ||
         url == "http://www.mypoquerenlinea.com/16195.html" ||
         url == "http://www.ionlinepoquer.com/onlinepoker12.html" ||
         url == "http://www.mypoquers.com/partypoquers-million.html" ||
         url == "http://www.bestpoquerenlinea.com/04469.php" ||
         url == "http://www.bestpoquerenlinea.com/11578.php";
}

void Parse(unsigned* total_documents, parsing::Parser* parser,
           RICPNS::CollectionReader* reader) {
  bool proceed = false;
  RICPNS::Document doc;

  proceed = reader->getNextDocument(doc);

  while (proceed) {
    std::string contents = doc.getText(), url = doc.getURL();
    if (parser->Convert(contents) && !parser->AlreadyRead(url)) {
      std::vector<std::pair<std::string, std::string>> referred_pages;
      std::unique_ptr<util::Page> page =
          std::move(parser->Parse(url, contents, referred_pages));
      if (!(page->text().size() <= 1)) {
        if (!parser->GenerateRankingData(page, referred_pages)) {
          cout << endl;
          std::cerr << "Something massively wrong happened, aborting"
                    << std::endl;
          return;
        }
      }
    }
    doc.clear();
    proceed = reader->getNextDocument(doc);

    if (*total_documents % 1000 == 0) {
      cout << "\rProcessed Documents: " << *total_documents;
      fflush(stdout);
    }
    (*total_documents)++;
  }
}

int main(int argc, char** argv) {
  if (argc < 8) {
    std::cout << argv[0]
              << " [input directory] [index file] [stop words file] "
                 "[vocabulary dump file] [document metadata file "
                 "path] [tuple file path] [anchor text tuple file path]"
              << endl;
    return 0;
  }

  std::fstream document_length_file;
  document_length_file.open(argv[5], std::fstream::binary | std::fstream::out);

  std::unique_ptr<parsing::Vocabulary> vocabulary(
      new parsing::Vocabulary(5000000));
  vocabulary->LoadStopWords(argv[3]);

  std::unique_ptr<ranking::RankingMetadata> ranking_metadata(
      new ranking::RankingMetadata(argv[5]));

  std::string index_file_path = argv[1];
  index_file_path += '/';
  index_file_path += argv[2];
  parsing::Parser parser(new ::util::FileManager(0, argv[6], argv[6]),
                         new ::util::FileManager(0, argv[7], argv[7]),
                         vocabulary.release(),
                         new ::parsing::PageKnowledge(index_file_path),
                         ranking_metadata.release());

  RICPNS::CollectionReader reader(argv[1], argv[2]);

  unsigned total_documents = 0;
  Parse(&total_documents, &parser, &reader);

  cout << "\rProcessed Documents: " << total_documents;
  cout << endl;
  parser.DumpVocabulary(argv[4]);
}
