#ifndef _RI_2015_1_UTIL_VOCABULARY_H_
#define _RI_2015_1_UTIL_VOCABULARY_H_

#include<string>
#include<vector>
#include<unordered_map>

class Vocabulary {
  public:
    Vocabulary() {};
    
    // Reads a list of strings and formats it to the underlying representation.
    void Init(const std::vector<std::string>& strings);

    // Add a new term |s| to the vocabulary. Returns true if successful, false otherwise.
    bool Insert(const std::string& s);
    
    // Returns true if |s| is in the vocabulary, false otherwise.
    bool Check(const std::string& s);
  
    // Writes the vocabulary to |file_path|.
    void Dump(const std::string& file_path);
    
    // Recovers the vocabulary written to |file_path| with Dump.
    void Load(const std::string& file_path);
    
  private:
    std::unordered_map<string, unsigned> vocabulary_;
}
#endif
