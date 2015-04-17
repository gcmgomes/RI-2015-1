#include "../tuple.h"
#include "../file_manager.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 3) {
    cout << argv[0] << " [tuple file] [memory limit]" << endl;
    return 0;
  }
  unsigned tuple_count = 0, memory_limit = 0;
  sscanf(argv[2], "%u", &memory_limit);
  tuple_count = util::FileManager::CountTuples(argv[1]);
  cout << util::Tuple::OptimalRunCount(memory_limit, tuple_count, 0) << endl;
}
