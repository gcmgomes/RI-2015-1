#include <iostream>
#include "../ranking_metadata.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << argv[0] << " [metadata file]" << endl;
    return 0;
  }
  ranking::RankingMetadata metadata_(argv[1]);
  metadata_.LoadPages();
}
