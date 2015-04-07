#ifndef _RI_2015_1_UTIL_TUPLE_H_
#define _RI_2015_1_UTIL_TUPLE_H_

#include <iostream>
#include <string>

namespace util {

struct Tuple {
  unsigned term;
  unsigned document;
  unsigned frequency;
  unsigned position;
  unsigned tuple_file_id;

  // Returns true iff |this| is strictly smaller then |b|.
  bool operator<(const Tuple& b) const;
  bool operator<(const Tuple& b);

  std::string ToString();
};

}  // namespace util

#endif
