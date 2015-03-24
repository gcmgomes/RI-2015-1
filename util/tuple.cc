#include "tuple.h"

// Returns true iff |a| is strictly smaller then |b|.
const bool Tuple::LessThen(const Tuple& a, const Tuple& b) const {
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
