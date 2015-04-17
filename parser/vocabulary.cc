#include "vocabulary.h"
#include <iostream>

namespace parsing {

Vocabulary::Vocabulary(unsigned expected_hash_size) {
  vocabulary_.reserve(expected_hash_size);
}

unsigned Vocabulary::InsertTerm(const std::string& key) {
  if (!CheckTerms(key) && !CheckStopWords(key)) {
    unsigned size = vocabulary_.size();
    vocabulary_[key] = ++size;
    return size;
  }
  return vocabulary_[key];
}

void Vocabulary::InsertTerm(const std::string& key, unsigned value) {
  vocabulary_[key] = value;
}

bool Vocabulary::CheckTerms(const std::string& key) {
  return vocabulary_.count(key);
}
unsigned Vocabulary::GetMappedValue(const std::string& key) {
  if (!CheckTerms(key)) {
    return 0;
  }
  return vocabulary_[key];
}

void Vocabulary::OpenBinaryIncrementalFile(const std::string& file_path) {
  output_file_.open(file_path.c_str(),
                    std::fstream::binary | std::fstream::out);
}

void Vocabulary::IncrementalBinaryTermDump(const std::string& key,
                                           unsigned value) {
  // If we don't have an output file, we can't write to it.
  if (!output_file_.is_open()) {
    return;
  }
  // making it a char saves 3 bytes per term. 1000000 terms -> -3MB.
  unsigned char size = key.size();
  output_file_.write(reinterpret_cast<const char*>(&size),
                     sizeof(unsigned char));
  output_file_.write(key.c_str(), size);
  output_file_.write(reinterpret_cast<const char*>(&value), sizeof(unsigned));
}

void Vocabulary::DumpTerms(const std::string& file_path) {
  std::ofstream output_file;
  output_file.open(file_path.c_str(), std::ofstream::out);
  auto i = vocabulary_.begin();
  while (i != vocabulary_.end()) {
    output_file << i->first << ' ' << i->second << std::endl;
    i++;
  }
}

void Vocabulary::LoadTerms(const std::string& file_path) {
  std::ifstream input_file;
  input_file.open(file_path.c_str(), std::ifstream::in);
  while (!input_file.eof()) {
    std::string key = "";
    unsigned position = 0;
    input_file >> key >> position;
    key.shrink_to_fit();
    this->InsertTerm(key);
  }
}

void Vocabulary::InsertStopWord(const std::string& key) {
  if (!CheckStopWords(key)) {
    stop_words_.insert(key);
  }
}

void Vocabulary::LoadBinaryTerms(
    const std::string& file_path,
    const std::unordered_map<unsigned, unsigned>& bridge) {
  std::ifstream input_file;
  input_file.open(file_path.c_str(), std::ifstream::binary);
  unsigned i = 0;
  while (!input_file.eof()) {
    unsigned char size = 0;
    unsigned value = 0;
    // Get key length.
    input_file.read(reinterpret_cast<char*>(&size), sizeof(unsigned char));

    std::string key((unsigned)size, 0);

    // Get key itself.
    input_file.read(reinterpret_cast<char*>(&key[0]), size * sizeof(char));
    key.shrink_to_fit();

    // Get stored value.
    input_file.read(reinterpret_cast<char*>(&value), sizeof(unsigned));
    if (!bridge.empty() && bridge.count(i)) {
      value = bridge.at(i);
    }
    this->InsertTerm(key, value);
    i++;
  }
}

bool Vocabulary::CheckStopWords(const std::string& key) {
  return stop_words_.count(key);
}

void Vocabulary::DumpStopWords(const std::string& file_path) {
  std::ofstream output_file;
  output_file.open(file_path.c_str(), std::ofstream::out);
  auto i = stop_words_.begin();
  while (i != stop_words_.end()) {
    output_file << *i << std::endl;
    i++;
  }
}

void Vocabulary::LoadStopWords(const std::string& file_path) {
  std::ifstream input_file;
  input_file.open(file_path.c_str(), std::ifstream::in);
  while (!input_file.eof()) {
    std::string key = "";
    input_file >> key;
    key.shrink_to_fit();
    this->InsertStopWord(key);
  }
}

}  // namespace parsing
