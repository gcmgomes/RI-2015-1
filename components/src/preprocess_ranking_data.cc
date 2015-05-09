#include "../retriever.h"
#include "../../ranking/page_ranker.h"
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
    cout << argv[0] << " [vocabulary file] [binary index file] [binary anchor index file] [rankng metadata file] <page rank parameter q (0.15)>" << endl;
    return 0;
  }
  double q = 0.15;
  if(argc > 5) {
    sscanf(argv[5], "%lf", &q);
  }
  components::Retriever* retriever = new components::Retriever(argv[2], argv[3], argv[4]);
  retriever->Init(argv[1], true);
  retriever->UpdatePagesToFile();
  delete retriever;
  ranking::PageRanker page_ranker(argv[4]);
  page_ranker.Init();
  page_ranker.Rank(q);
  page_ranker.UpdatePagesToFile();
}
