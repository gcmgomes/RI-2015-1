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

void Vocabulary::Dump(const std::string& file_path) {
  std::ofstream output_file;
  output_file.open(file_path.c_str(), std::ofstream::binary);
  auto i = vocabulary_,begin();
  while(i != vocabulary_.end()) {
    // Writes the amount of bytes present in the string.
    output_file.write(reinterprete_cast<const char*>(&i->first.size()), sizeof(unsigned));
    // Writes the string itself.
    output_file.write(i->first.c_str(), i->first.size());
    // Writes the term_id of the current string.
    output_file.write(reinterprete_cast<const char*>(&i->second), sizeof(unsigned));
    i++;
  }
}

void Vocabulary::Load(const std::string& file_path) {
  std::ifstream input_file;
  input_file.open(file_path.c_str(), std::ifstream::binary);
  while(!input_file.eof()) {
    // Reads the amount of bytes present in the string.
    unsigned size = 0;
    input_file.read(sizeof(unsigned), reinterprete_cast<const char*>(&size));
    // Reads the string itself.
    char* str = new char[size];
    input_file.read(size, str);
    std::string key = str;
    this->Insert(key);
  }
}
