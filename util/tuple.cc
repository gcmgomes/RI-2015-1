#include <cstdio>
#include <cstring>
#include <memory>

#include "tuple.h"

namespace util {

unsigned long long Tuple::OptimalRunCount(unsigned long long memory_limit,
                                          unsigned long long total_tuples,
                                          bool verbose) {
  // If limit < 2MB we can't do a whole lot with large collections, so just
  // force the limit higher.
  if (memory_limit < 2000000) {
    memory_limit = 2000000;
    if (verbose) {
      std::cerr << "Minimum needed memory is 2MB (2000000)" << std::endl;
    }
  }
  // We need around 1MB of support memory, so lets ditch from the limit.
  memory_limit -= 1000000;
  unsigned long long memory_per_tuple =
      sizeof(std::unique_ptr<Tuple>) + sizeof(Tuple);
  unsigned long long sortable_tuples = memory_limit / memory_per_tuple;
  unsigned long long runs_count = total_tuples / sortable_tuples;
  if (verbose) {
    std::cerr << "1MB needed for support structures" << std::endl;
    std::cerr << "Available memory: " << memory_limit << std::endl;
    std::cerr << "Memory per tuple: " << memory_per_tuple << std::endl;
    std::cerr << "Tuples to sort  : " << total_tuples << std::endl;
    std::cerr << "Sortable tuples : " << sortable_tuples << std::endl;
  }
  return ++runs_count;
}

bool Tuple::operator<(const Tuple& b) const {
  const Tuple a = *this;
  if (a.term < b.term) {
    return true;
  }

  if (a.term > b.term) {
    return false;
  }

  // a.term == b.term, compare document index.
  if (a.document < b.document) {
    return true;
  }
  if (a.document > b.document) {
    return false;
  }

  // a.document == b.document, compare position (frequencies are ==).
  if (a.position < b.position) {
    return true;
  }
  // |a| is not strictly smaller then |b|.
  return false;
}

bool Tuple::operator<(const Tuple& b) {
  const Tuple* t = this;
  return (*t) < b;
}

std::string Tuple::ToString() const {
  std::string str = "";
  char i[128];
  sprintf(i, "%u ", this->term);
  str += i;
  sprintf(i, "%u ", this->document);
  str += i;
  sprintf(i, "%u ", this->frequency);
  str += i;
  sprintf(i, "%u", this->position);
  str += i;
  return str;
}

}  // namespace util
