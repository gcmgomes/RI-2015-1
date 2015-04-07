#include <cstdio>
#include <cstring>

#include "tuple.h"

namespace util {

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

std::string Tuple::ToString() {
  std::string str = "";
  char i[128];
  sprintf(i, "term: %u\n", this->term);
  str += i;
  sprintf(i, "document: %u\n", this->document);
  str += i;
  sprintf(i, "frequency: %u\n", this->frequency);
  str += i;
  sprintf(i, "position: %u", this->position);
  str += i;
  return str;
}

}  // namespace util
