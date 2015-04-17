#include "../parser.h"
#include "../vocabulary.h"
#include "../../util/page.h"
#include "../ricp/src/Document.h"
#include "../ricp/src/CollectionReader.h"
#include "../../util/file_manager.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 6) {
    std::cout << argv[0] << " [input directory] [index file] [stop words file] "
                            "[binary vocabulary dump file] [tuple file path]\n";
    return 0;
  }

  std::unique_ptr<parsing::Vocabulary> vocabulary(
      new parsing::Vocabulary(5000000));
    vocabulary->LoadStopWords(argv[3]);
    vocabulary->OpenBinaryIncrementalFile(argv[4]);

  parsing::Parser parser(new ::util::FileManager(0, argv[5], argv[5]),
                         vocabulary.release());

  RICPNS::CollectionReader* reader =
      new RICPNS::CollectionReader(argv[1], argv[2]);
  RICPNS::Document doc;

  unsigned total_documents = 0;
  while (reader->getNextDocument(doc)) {
    std::string contents = doc.getText(), url = doc.getURL();
    doc.clear();
    parser.Convert(contents);
    std::unique_ptr<util::Page> page = std::move(parser.Parse(url, contents));
    if (!parser.GenerateTuples(page, total_documents++)) {
      cout << endl;
      std::cerr << "Something massively wrong happened, aborting" << std::endl;
      return -1;
    }
    if (total_documents % 1000 == 0) {
      cout << "\rProcessed Documents: " << total_documents;
      fflush(stdout);
    }
  }
  cout << endl;
  delete reader;
}
