#ifndef _RI_2015_1_COMPONENTS_INDEX_ENTRY_H_
#define _RI_2015_1_COMPONENTS_INDEX_ENTRY_H_

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

namespace components {
class Indexer;

class IndexEntry {
 friend class Indexer;

 public:
  IndexEntry() {
    term_ = 0;
  }

  unsigned term() const {
    return term_;
  };

  unsigned total_frequency() const {
    return total_frequency_;
  }

  const std::map<unsigned, std::vector<unsigned> >& occurrences() const {
    return occurrences_;
  };

  std::map<unsigned, std::vector<unsigned> >& mutable_occurrences() {
    return occurrences_;
  };

  std::string ToString() {
    std::stringstream str;
    str << term_ << " | (" << total_frequency_ << ") " << occurrences_.size() << " {";
    auto i = occurrences_.begin();
    while(i != occurrences_.end()) {
      str << "[" << i->first << ", " << i->second.size() << "(";
      auto j = i->second.begin();
      str << *j;
      ++j;
      while(j != i->second.end()) {
        str << ", ";
        str << *j;
        ++j;
      }
      str << ")]";
      ++i;
      if(i != occurrences_.end()) {
        str << ", ";
      }
    }
    str << "}";
    return str.str();
  }

 private:
  unsigned term_;
  unsigned total_frequency_;
  // This is a mapping document_id -> position_vector.
  std::map<unsigned, std::vector<unsigned> > occurrences_;
};
}  // namespace components

#endif
