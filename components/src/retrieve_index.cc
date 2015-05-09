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
  if (argc < 5) {
    cout << argv[0] << " [vocabulary file] [binary index file] [binary anchor index file] [rankng metadata file] <ranking model (0)> <beta weighting(0.5)>" << endl;
    return 0;
  }

  double beta = 0.5;
  unsigned model = 0;
  if(argc > 5) {
    sscanf(argv[5], "%u", &model);
  }

  if(argc > 6) {
    sscanf(argv[6], "%lf", &beta);
  }

  components::Retriever* retriever = new components::Retriever(argv[2], argv[3], argv[4]);
  retriever->Init(argv[1], false);
  ranking::Ranker ranker(retriever, beta);

  while(true) {
    std::string query = "";
    cout << "Waiting query" << endl;
    std::getline(cin, query);
    if(cin.eof()) {
      break;
    }
    std::vector<::util::Page> answers;
    ranker.retriever()->Retrieve(query, answers);
    ranker.Rank(query, answers, model);
    auto i = answers.begin();
    while(i != answers.end()) {
      cout << i->ToString() << endl;
      ++i;
    }
    cout << endl;
  }
  cout << "Goodbye" << endl;
}
