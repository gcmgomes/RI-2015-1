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
  if (argc < 3) {
    cout << argv[0] << " [number of runs generate] [runs file prefix] <doc "
                       "count (10000)> <unique term count (10000)> <SKIP>"
         << endl;
    return 0;
  }
  unsigned doc_count = 10000, doc_size = 0, unique_term_count = 10000;
  unsigned runs_count = 0;
  unsigned long long tuple_count = 0;
  bool skip_gen = 0;
  sscanf(argv[1], "%u", &runs_count);
  if (argc > 3) {
    sscanf(argv[3], "%u", &doc_count);
    if (doc_count == 0) {
      doc_count = 10000;
    }
  }
  if (argc > 4) {
    sscanf(argv[4], "%u", &unique_term_count);
    if (unique_term_count == 0) {
      unique_term_count = 10000;
    }
  }

  if (argc > 5) {
    sscanf(argv[5], "%u", &doc_size);
    skip_gen = doc_size > 2;
  }

  if (!skip_gen) {
    util::FileManager* file_manager = new util::FileManager(
        runs_count, string(argv[2]), string(argv[2]) + ".full");
    // Documents range in [0,100).
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
    file_manager->CloseOutput();
    delete file_manager;
  }

  else {
    util::FileManager* file_manager = new util::FileManager(
        runs_count, string(argv[2]), string(argv[2]) + ".full.dummy");
    file_manager->Split(string(argv[2]) + ".full");
    file_manager->CloseOutput();
    delete file_manager;
  }
  return 0;
}
