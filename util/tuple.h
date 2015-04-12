#ifndef _RI_2015_1_UTIL_TUPLE_H_
#define _RI_2015_1_UTIL_TUPLE_H_

#include <iostream>
#include <memory>
#include <string>

namespace util {

struct Tuple {
  unsigned term;
  unsigned document;
  unsigned frequency;
  unsigned position;
  unsigned tuple_file_id;

  Tuple(unsigned term_, unsigned document_, unsigned frequency_,
        unsigned position_)
      : term(term_),
        document(document_),
        frequency(frequency_),
        position(position_),
        tuple_file_id(0) {};

  Tuple(){};

  // Returns true iff |this| is strictly smaller then |b|.
  bool operator<(const Tuple& b) const;
  bool operator<(const Tuple& b);

  std::string ToString() const;
};

// Comparator class.
struct TupleCompare {
  bool operator()(const Tuple& a, const Tuple& b) {
    return a < b;
  }

  bool operator()(const std::unique_ptr<Tuple>& a,
                  const std::unique_ptr<Tuple>& b) {
    return *b < *a;
  }

  static bool TuplePointerCompare(const std::unique_ptr<Tuple>& a,
                                  const std::unique_ptr<Tuple>& b) {
    return *a < *b;
  }
};

}  // namespace util

#endif
