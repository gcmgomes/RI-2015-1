#include "../ranking_preprocessor.h"
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
    cout << argv[0] << " [input rankng metadata file] [output ranking metadata file] [index file] [anchor index file ] <page rank parameter q (0.15)> <only page rank>" << endl;
    return 0;
  }
  double q = 0.15;
  if(argc > 5) {
    sscanf(argv[5], "%lf", &q);
  }
  ranking::RankingPreprocessor preprocessor(argv[1], argv[2], argv[3], argv[4], q);
  preprocessor.Preprocess(argc > 6);
}
