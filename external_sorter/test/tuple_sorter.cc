#include "../sorter.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 4) {
    cout << "[number of runs to sort] [runs file prefix] [output file]" << endl;;
    return 0;
  }

  unsigned runs_count = 0;
  sscanf(argv[1], "%u", &runs_count);
  util::FileManager* f = new util::FileManager(runs_count, string(argv[2]),
                                               string(argv[3]));
  sorting::Sorter s(f);
  s.ExternalMultiwayMerge();
  return 0;
}
