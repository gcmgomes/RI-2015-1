#ifndef _RI_2015_1_UTIL_TUPLE_H_
#define _RI_2015_1_UTIL_TUPLE_H_

#include <iostream>

struct Tuple {
  unsigned term;
  unsigned document;
  unsigned frequency;
  unsigned position;

  static const bool LessThen(const Tuple& a, const Tuple& b) const;
};

#endif
