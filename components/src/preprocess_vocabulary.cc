#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <set>
#include <memory>
#include <unordered_set>
#include "../retriever.h"
#include "../../ranking/ranker.h"
#include "../../ranking/query_evaluator.h"
#include "../../util/page.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc < 5) {
    cout << argv[0] << " [vocabulary file] [binary index file] [binary anchor "
                       "index file] [remapped vocabulary file]" << endl;
    return 0;
  }

  components::Retriever* retriever =
      new components::Retriever(argv[2], argv[3], "");
  retriever->Init(argv[1], true);
  retriever->vocabulary()->Dump(argv[4]);
}
