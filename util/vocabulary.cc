#include "vocabulary.h"
#include <fstream>

namespace util {

void Vocabulary::Init(const std::vector<std::string>& strings) {
  unsigned i = 0;
  while (i < strings.size()) {
    vocabulary_[strings[i].c_str()] = i;
    i++;
  }
}

void Vocabulary::Insert(const std::string& s) {
  if (!Check(s)) {
    unsigned size = vocabulary_.size();
    vocabulary_[s.c_str()] = ++size;
  }
}

bool Vocabulary::Check(const std::string& s) {
  return vocabulary_.count(s.c_str());
}

void Vocabulary::Dump(const std::string& file_path) {
  std::ofstream output_file;
  output_file.open(file_path.c_str(), std::ofstream::binary);
  std::unordered_map<const char*, unsigned long long>::iterator i =
      vocabulary_.begin();
  while (i != vocabulary_.end()) {
    // Writes the amount of bytes present in the string.i
    output_file.write(reinterpret_cast<const char*>(strlen(i->first), sizeof(unsigned));
    // Writes the string itself.
    output_file.write(i->first, strlen(i->first.size()));
    // Writes the term_id of the current string.
    output_file.write(reinterpret_cast<const char*>(&i->second), sizeof(unsigned));
    i++;
  }
}

void Vocabulary::Load(const std::string& file_path) {
  std::ifstream input_file;
  input_file.open(file_path.c_str(), std::ifstream::binary);
  while (!input_file.eof()) {
    // Reads the amount of bytes present in the string.
    unsigned size = 0;
    input_file.read(sizeof(unsigned), reinterpret_cast<const char*>(&size));
    // Reads the string itself.
    char* str = new char[size];
    input_file.read(size, str);
    std::string key = str;
    delete str;
    this->Insert(key);
  }
}

}  // namespace util
