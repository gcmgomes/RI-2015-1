#include "vocabulary.h"
#include <iostream>

namespace parsing {

Vocabulary::Vocabulary(unsigned expected_hash_size) {
  vocabulary_.reserve(expected_hash_size);
}

VocabularyEntry Vocabulary::Insert(const std::string& key) {
  if (!Check(key) && !CheckStopWords(key)) {
    unsigned size = vocabulary_.size();
    vocabulary_[key] = std::make_pair(size, size);
    return std::make_pair(size, size);
  }
  return vocabulary_[key];
}

void Vocabulary::Insert(const std::string& key, VocabularyEntry value) {
  vocabulary_[key] = value;
}

bool Vocabulary::Check(const std::string& key) const {
  return vocabulary_.count(key);
}

VocabularyEntry Vocabulary::GetMappedValue(const std::string& key) const {
  if (!Check(key)) {
    return std::make_pair(0, 0);
  }
  return vocabulary_.at(key);
}

void Vocabulary::Dump(const std::string& file_path) const {
  std::ofstream output_file;
  output_file.open(file_path.c_str(), std::ofstream::out);
  auto i = vocabulary_.begin();
  while (i != vocabulary_.end()) {
    output_file << i->first << ' ' << i->second.first << ' ' << i->second.second
                << std::endl;
    i++;
  }
}

void Vocabulary::Load(const std::string& file_path,
                      const std::unordered_map<unsigned, VocabularyEntry>& bridge) {
  std::ifstream input_file;
  input_file.open(file_path.c_str(), std::ifstream::in);
  while (!input_file.eof() && input_file.peek() != EOF) {
    std::string key = "";
    unsigned position = 0, anchor_position = 0;
    input_file >> key >> position >> anchor_position;
    if (input_file.eof()) {
      break;
    }
    key.shrink_to_fit();
    if (!bridge.empty() && bridge.count(position)) {
      position = bridge.at(position).first;
      anchor_position = bridge.at(anchor_position).second;
    }
    this->Insert(key, std::make_pair(position, anchor_position));
  }
  using namespace std;
}

void Vocabulary::InsertStopWord(const std::string& key) {
  if (!CheckStopWords(key)) {
    stop_words_.insert(key);
  }
}

bool Vocabulary::CheckStopWords(const std::string& key) const {
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
