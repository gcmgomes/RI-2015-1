#include "vocabulary.h"
#include <fstream>

void Vocabulary::Init(const std::vector<std::string>& strings) {
  unsigned i = 0;
  while(i < strings.size()) {
    vocabulary_[strings[i]] = i+1;
    i++;
  }
}

void Vocabulary::Insert(const std::string& s) {
  if(!Check(s)) {
    unsigned size = vocabulary.size();
    vocabulary_[s] = ++size;
  }
}

bool Vocabulary::Check(const std::string& s) {
  return vocabulary_.count(s);
}

void Dump(const std::string& file_path) {
  std::ofstream output_file;
  output_file.open(file_path.c_str(), std::ofstream::binary);
  std::unordered_map::iterator i = vocabulary_,begin();
  while(i != vocabulary_.end()) {
    // Writes the amount of bytes present in the string.
    output_file.write((char*) i->first.size(), 4*sizeof(size_t));
    // Writes the string itself.
    output_file.write(i->first.c_str(), i->first.size());
    // Writes the term_id of the current string.
    output_file.write((char*) i->second, 4*sizeof(unsigned));
    i++;
  }
}
