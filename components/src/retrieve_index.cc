#include "../retriever.h"
#include "../../ranking/ranker.h"
#include "../../util/page.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <memory>
#include <unordered_set>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 4) {
    cout << argv[0] << " [binary vocabulary] [binary index file] [rankng metadata file]" << endl;
    return 0;
  }

  components::Retriever* retriever = new components::Retriever(argv[2], argv[3]);
  retriever->Init(argv[1]);
  ranking::Ranker ranker(retriever);


  while(true) {
    std::string query = "";
    cout << "Waiting query" << endl;
    std::getline(cin, query);
    if(cin.eof()) {
      break;
    }
    std::vector<::util::Page> answers;
    ranker.retriever()->Retrieve(query, answers);
    ranker.Rank(query, answers, 0);
    auto i = answers.begin();
    while(i != answers.end()) {
      cout << i->ToString() << endl;
      ++i;
    }
    cout << endl;
  }
  cout << "Goodbye" << endl;
}
