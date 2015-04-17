#ifndef _RI_2015_1_UTIL_VOCABULARY_H_
#define _RI_2015_1_UTIL_VOCABULARY_H_

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace parsing {

class Vocabulary {
 public:
  Vocabulary();

  Vocabulary(unsigned expected_hash_size);
  // Add a new term |key| to the vocabulary. Returns the mapped value.
  unsigned InsertTerm(const std::string& key);

  // Add a new term |key| to the vocabulary.
  void InsertTerm(const std::string& key, unsigned value);

  // Returns true if |key| is in the vocabulary, false otherwise.
  bool CheckTerms(const std::string& key);

  // Returns the value to which |key is mapped|. The user must issue a call to
  // CheckTerms(key) to see if the 0 returned is, in fact, valid.
  unsigned GetMappedValue(const std::string& key);

  // Open |output_file_| for output.
  void OpenBinaryIncrementalFile(const std::string& file_path);

  // Dump the pair (|key|, |value|) as they are inserted. If done right, this
  // preserves input order!
  void IncrementalBinaryTermDump(const std::string& key, unsigned value);

  // Writes the vocabulary to |file_path|.
  void DumpTerms(const std::string& file_path);

  // Recovers the vocabulary written to |file_path| with DumpTerms().
  void LoadTerms(const std::string& file_path);

  // Recovers the binary vocabulary written to |file_path| with
  // IncrementalBinaryTermDump(). |bridge| acts as an updater for the mapped
  // value.
  void LoadBinaryTerms(const std::string& file_path,
                       const std::unordered_map<unsigned, unsigned>& bridge);

  // Add a new stop word to be ignored by the vocabulary.
  void InsertStopWord(const std::string& key);

  // Returns true if |key| is a stop word, false otherwise.
  bool CheckStopWords(const std::string& key);

  // Writes the stop words to |file_path|.
  void DumpStopWords(const std::string& file_path);

  // Recovers the stop words written to |file_path| with DumpStopWords().
  void LoadStopWords(const std::string& file_path);

 private:
  std::unordered_map<std::string, unsigned> vocabulary_;

  std::unordered_set<std::string> stop_words_;

  std::fstream output_file_;
};

}  // namespace parsing
#endif
