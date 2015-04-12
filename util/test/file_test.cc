#include "../tuple.h"
#include "../file_manager.h"
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

int main(int argc, char** argv) {
  util::Tuple t;
  std::unique_ptr<util::Tuple> t1 = nullptr;
  util::FileManager f(1, string(argv[1]) + "test.bin",
                      string(argv[1]) + "test.bin0");
  unsigned i = 0;
  while (i < 10) {
    t.term = i+10;
    t.document = i+20;
    t.frequency = i+30;
    t.position = i+40;
    t.tuple_file_id = i+50;
    f.WriteTuple(&t);
    cout << t.ToString() << endl;
    i++;
  }
  i--;
  f.CloseOutput();
  while (true) {
    t1 = std::move(f.GetNextTuple(0));
    if(t1 == NULL) {
      break;
    }
    t = *t1.release();
    cout << t.ToString() << endl;
  }
  return 0;
}
