#include "../file_manager.h"
#include "../indexer.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 3) {
    cout << argv[0] << " [binary input file] [textual output file] <is index file>" << endl;
    return 0;
  }
  if(argc < 4) {
    util::FileManager::ConvertBinToText(argv[1], argv[2]);
  }
  else {
    cout << "Converting index" << endl;
    util::Indexer::ConvertIndexToText(argv[1], argv[2]);
  }
  return 0;
}
