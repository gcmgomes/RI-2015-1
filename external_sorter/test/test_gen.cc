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
    cout << "[number of runs generate] [runs file prefix]" << endl;
    return 0;
  }
  unsigned doc_count = 100000, doc_size = 0, unique_term_count = 10000;
  unsigned runs_count = 0, tuple_count = 0;
  sscanf(argv[1], "%u", &runs_count);

  util::FileManager* file_manager = new util::FileManager(
      runs_count, string(argv[2]), string(argv[2]) + ".full");

  // Documents range in [0,100).
  while (doc_count) {
    cout << "\rRemaining documents: " << doc_count;
    fflush(stdout);
    doc_count--;
    doc_size = rand() % (unique_term_count/10);
    vector<util::Tuple>* tuples =
        GenerateDocument(doc_count, doc_size, unique_term_count);
    tuple_count += tuples->size();
    // Document size ranges in [0, doc_size).
    while (doc_size) {
      doc_size--;
      util::Tuple* t = NULL;
      t = &(*tuples)[doc_size];
      file_manager->WriteTuple(t);
    }
    delete tuples;
  }

  cout << endl << "Generated " << tuple_count << " tuples" << endl;
  file_manager->CloseOutput();
  file_manager->Split(string(argv[2]) + ".full");
  delete file_manager;
  return 0;
}
