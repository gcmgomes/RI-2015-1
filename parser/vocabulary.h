#ifndef _RI_2015_1_UTIL_VOCABULARY_H_
#define _RI_2015_1_UTIL_VOCABULARY_H_

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace parsing {

typedef std::pair<unsigned, unsigned> VocabularyEntry;

class Vocabulary {
 public:
  Vocabulary();

  Vocabulary(unsigned expected_hash_size);

  // Add a new term |key| to the vocabulary. Returns the mapped value.
  VocabularyEntry Insert(const std::string& key);

  // Add a new term |key| to the vocabulary.
  void Insert(const std::string& key, VocabularyEntry value);

  // Returns true if |key| is in the vocabulary, false otherwise.
  bool Check(const std::string& key) const;

  // Returns the value to which |key is mapped|. The user must issue a call to
  // Check(key) to see if the 0 returned is, in fact, valid.
  VocabularyEntry GetMappedValue(const std::string& key) const;

  // Writes the vocabulary to |file_path|.
  void Dump(const std::string& file_path) const;

  // Recovers the vocabulary written to |file_path| with Dump(). Uses
  // |bridge| to update the read values.
  void Load(const std::string& file_path,
            const std::unordered_map<unsigned, VocabularyEntry>& bridge);

  // Writes the stop words to |file_path|.
  void DumpStopWords(const std::string& file_path);

  // Recovers the stop words written to |file_path| with DumpStopWords().
  void LoadStopWords(const std::string& file_path);

  // Add a new stop word to be ignored by the vocabulary.
  void InsertStopWord(const std::string& key);

  // Returns true if |key| is a stop word, false otherwise.
  bool CheckStopWords(const std::string& key) const;

 private:
  std::unordered_map<std::string, VocabularyEntry> vocabulary_;

  std::unordered_set<std::string> stop_words_;
};

}  // namespace parsing
#endif
