#ifndef _RI_2015_1_COMPONENTS_INDEX_ENTRY_H_
#define _RI_2015_1_COMPONENTS_INDEX_ENTRY_H_

#include <iostream>
#include <map>
#include <vector>

namespace components {
class Indexer;

class IndexEntry {
 friend class Indexer;

 public:
  unsigned term() {
    return term_;
  };
  const std::map<unsigned, std::vector<unsigned> >& occurences() {
    return occurences_;
  };

 private:
  unsigned term_;
  // This is a mapping document_id -> position_vector.
  std::map<unsigned, std::vector<unsigned> > occurences_;
};
}  // namespace components

#endif
