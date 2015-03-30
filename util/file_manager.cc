#include <cstring>
#include <cstdio>
#include "file_manager.h"

namespace util {

FileManager::FileManager(unsigned file_count, std::string file_prefix, std::string output_file_path) {
  file_prefix_ = file_prefix;
  input_files_.assign(file_count, NULL));
  output_file_.open(output_file_path.c_str(), std::ofstream::binary);
}

FileManager::~FileManager() {
  unsigned i = 0;
  while(i < input_files_.size()) {
    if(input_files_[i] != NULL) {
      delete input_files_[i];
    }
    i++;
  }
  output_file_.close();
}

Tuple* FileManager::GetNextTuple(unsigned file_id) {
  // Lazy input file initialization.
  if(input_files_[file_id] == NULL) {
    char file_suffix[16];
    sprintf(file_suffix, "%u", file_id);
    
    const std::string file_path = file_prefix_ + file_sufix;
    input_files_[file_id] = new std::ifstream();
    input_files_[file_id]->open(file_path.c_str(), std::ifstream::binary);
  }
  
  if(!input_files_[file_id].is_open()) {
    return NULL;
  }
  
  Tuple* new_tuple = new Tuple();
  input_files_[file_id]->read((char*) *new_tuple, 4*sizeof(unsigned));
  new_tuple.tuple_file_id = file_id;
  
  // Check if file is done and close it.
  if(input_files_[file_id]->peek() == EOF || input_files[file_id]->eof()) {
    input_files_[file_id]->close().
  }
  
  return new_tuple;
}

void FileManager::OutputTuple(Tuple* tuple) {
  // Writes the first 4 unsigned fields of |tuple| to |output_file_|.
  output_file_.write((char*) *tuple,  4*sizeof(unsigned))
}

void FileManager::InitializeHeap(std::priority_queue<Tuple, vector<Tuple>, &Tuple::LessThen>* heap) {
  unsigned file_id = 0;
  while(this->input_files_.size()) {
    Tuple* tuple = this->GetNextTuple(file_id);
    if(tuple != NULL) {
      heap->push(*tuple);
      delete tuple;
    }
    file_id++;
  }
}

} // namespace util
