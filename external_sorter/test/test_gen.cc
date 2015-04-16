#include "../sorter.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

vector<util::Tuple>* GenerateDocument(unsigned doc_id, unsigned doc_size,
                                      unsigned unique_term_count) {
  vector<util::Tuple>* tuples = new vector<util::Tuple>();
  unordered_map<unsigned, unsigned> frequencies;
  unsigned position = 0;
  while (position < doc_size) {
    util::Tuple t;
    // Generate tuple data.
    t.term = rand() % unique_term_count;
    t.document = doc_id;
    t.frequency = 0;
    t.position = position;
    frequencies[t.term]++;

    tuples->push_back(t);
    position++;
  }

  while (position) {
    position--;
    (*tuples)[position].frequency = frequencies[(*tuples)[position].term];
  }
  return tuples;
}

int main(int argc, char** argv) {
  if (argc < 4) {
    cout << argv[0]
         << "[runs file prefix] <doc count (10000)> <unique term count (10000)>"
         << endl;
    return 0;
  }
  unsigned doc_count = 10000, doc_size = 0, unique_term_count = 10000;
  unsigned long long tuple_count = 0;

  sscanf(argv[2], "%u", &doc_count);
  if (doc_count == 0) {
    doc_count = 10000;
  }

  sscanf(argv[3], "%u", &unique_term_count);
  if (unique_term_count == 0) {
    unique_term_count = 10000;
  }

  util::FileManager* file_manager =
      new util::FileManager(1, string(argv[1]), string(argv[1]));

  // Documents range in [0,doc_count).
  cout << doc_count << ' ' << unique_term_count << endl;
  while (doc_count) {
    doc_count--;
    doc_size = rand() % (unique_term_count / 10);
    vector<util::Tuple>* tuples =
        GenerateDocument(doc_count, doc_size, unique_term_count);
    tuple_count += tuples->size();
    fflush(stdout);
    // Document size ranges in [0, doc_size).
    while (doc_size) {
      doc_size--;
      util::Tuple* t = NULL;
      t = &(*tuples)[doc_size];
      file_manager->WriteTuple(t);
    }
    delete tuples;
  }

  cout << "Generated tuples: " << tuple_count << endl;
  file_manager->CloseOutput();

  delete file_manager;
  return 0;
}
