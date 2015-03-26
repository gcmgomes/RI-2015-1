#ifndef _RI_2015_1_UTIL_TUPLE_H_
#define _RI_2015_1_UTIL_TUPLE_H_

#include <iostream>

namespace util {

struct Tuple {
  unsigned term;
  unsigned document;
  unsigned frequency;
  unsigned position;
  unsigned tuple_file;
  unsigned tuple_file_position;
  
  static const bool LessThen(const Tuple& a, const Tuple& b) const;
};

#endif

} // namespace util
