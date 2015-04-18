#include "vocabulary.h"
#include <iostream>

namespace parsing {

Vocabulary::Vocabulary(unsigned expected_hash_size) {
  vocabulary_.reserve(expected_hash_size);
}

unsigned Vocabulary::InsertTerm(const std::string& key) {
  if (!CheckTerms(key) && !CheckStopWords(key)) {
    unsigned size = vocabulary_.size();
    vocabulary_[key] = size;
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

void Vocabulary::DumpTerms(const std::string& file_path) {
  std::ofstream output_file;
  output_file.open(file_path.c_str(), std::ofstream::out);
  auto i = vocabulary_.begin();
  while (i != vocabulary_.end()) {
    output_file << i->first << ' ' << i->second << std::endl;
    i++;
  }
}

void Vocabulary::LoadTerms(
    const std::string& file_path,
    const std::unordered_map<unsigned, unsigned>& bridge) {
  std::ifstream input_file;
  input_file.open(file_path.c_str(), std::ifstream::in);
  while (!input_file.eof() && input_file.peek() != EOF) {
    std::string key = "";
    unsigned position = 0;
    input_file >> key >> position;
    if(input_file.eof()) {
      break;
    }
    key.shrink_to_fit();
    if(!bridge.empty() && bridge.count(position)) {
      position = bridge.at(position);
    }
    this->InsertTerm(key, position);
  }
  using namespace std;
  cout << "Bridge: " << bridge.size() << endl;
  cout << "Vocab:  " << vocabulary_.size() << endl;
}

void Vocabulary::InsertStopWord(const std::string& key) {
  if (!CheckStopWords(key)) {
    stop_words_.insert(key);
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
