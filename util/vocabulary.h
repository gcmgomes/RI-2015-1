#include<string>
#include<vector>
#include<tr1/unordered_map>

class Vocabulary {
  public:
    Vocabulary();
    
    // Reads a list of strings and formats it to the underlying representation.
    void Init(const std::vector<std::string>* strings);

    // Add a new term |s| to the vocabulary. Returns true if successful, false otherwise.
    bool Insert(std::string s);
    
    // Returns true if |s| is in the vocabulary, false otherwise.
    bool Check(std::string s);
  
    // Writes the vocabulary to |file_path| using blocks with |block_size| strings each.
    void EntryBlockingDump(std::string file_path, unsigned block_size);
    
  private:
    // Hash table. May change later on.
    std::tr1::unordered_map<string, int> vocabulary_;
}
