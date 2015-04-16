#include "../sorter.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 3) {
    cout << argv[0] << " [number of blocks to sort] [tuples file]" << endl;
    return 0;
  }
  unsigned runs_count = 0;
  sscanf(argv[1], "%u", &runs_count);

  util::FileManager* file_manager = new util::FileManager(
      runs_count, string(argv[2]), "");

  file_manager->PartialEmplaceSort();

  delete file_manager;
  return 0;
}
