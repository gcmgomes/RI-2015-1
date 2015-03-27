#include <cstring>
#include <cstdio>
#include "file_manager.h"

namespace util {

FileManager::FileManager(unsigned file_count, std::string file_prefix, std::string output_file_path) {
  file_count_ = file_count;
  file_prefix_ = file_prefix;
  output_file_.open(output_file_path.c_str(), std::ofstream::binary);
}

FileManager::~FileManager() {
  input_file_.close();
  output_file_.close();
}

Tuple* FileManager::GetNextTuple(const Tuple* tuple) {
  if(tuple.tuple_file_id != input_file_id_) {
    input_file_id_ = tuple.tuple_file_id;
    input_file_.close();
    
    char file_id[16];
    sprintf(file_id, "%u", tuple.tuple_file_id);
    
    const std::string file_path = file_prefix_ + file_id;
    input_file_.open(file_path.c_str(), std::ifstream::binary);
    
    // Each tuple is written as 4 unsigned (4 bytes each) values.
    // Therefore, the n-th tuple starts at the (4*4*n)-byte.
    input_file_.seekg(4*4*tuple.tuple_id);
  }
  
  if(ifstream.eof() || ifstream.peek() == EOF) {
    return nullptr;
  }
  
  Tuple* new_tuple = new Tuple();
  input_file_.read((char*) *new_tuple, 4*sizeof(unsigned));
  new_tuple.tuple_file_id = tuple.input_file_id_;
  new_tuple.next_tuple_id = tuple.tuple_id+1;
  
  return new_tuple;
}

void OutputTuple(Tuple* tuple) {
  // Writes the first 4 unsigned fields of |tuple| to |output_file_|.
  output_file_.write((char*) *tuple,  4*sizeof(unsigned))
}

void FileManager::InitializeHeap(std::priority_queue<Tuple, vector<Tuple>, &Tuple::LessThen>* heap) {
  unsigned file_id = file_count_-1;
  while(file_id) {
    Tuple* tuple = this->GetNextTuple(file_id, 0);
    if(tuple != NULL) {
      heap->push(*tuple);
      delete tuple;
    }
    file_id--;
  }
}

} // namespace util
