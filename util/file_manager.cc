#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "file_manager.h"

namespace util {

FileManager::FileManager(unsigned file_count, std::string file_prefix,
                         std::string output_file_path) {
  std::cerr << "This is FileManager's constructor" << std::endl;
  std::cerr << "  file_count_  = " << file_count << std::endl;
  std::cerr << "  file_prefix  = " << file_prefix << std::endl;
  std::cerr << "  output_file_ = " << output_file_path << std::endl;
  file_prefix_ = file_prefix;
  input_files_.assign(file_count, NULL);
  output_file_.open(output_file_path.c_str(), std::ofstream::binary);
}

FileManager::~FileManager() {
  unsigned i = 0;
  while (i < input_files_.size()) {
    if (input_files_[i] != NULL) {
      delete input_files_[i];
    }
    i++;
  }
  output_file_.close();
}

static Tuple* GetTuple(std::ifstream* input_file) {
  Tuple* new_tuple = new Tuple();
  unsigned val = 322312;
  input_file->read(reinterpret_cast<char*>(&val), sizeof(unsigned));
  new_tuple->term = val;
  input_file->read(reinterpret_cast<char*>(&val), sizeof(unsigned));
  new_tuple->document = val;
  input_file->read(reinterpret_cast<char*>(&val), sizeof(unsigned));
  new_tuple->frequency = val;
  input_file->read(reinterpret_cast<char*>(&val), sizeof(unsigned));
  new_tuple->position = val;
  return new_tuple;
}

static void OutputTuples(const std::string& output_file_path,
                         const std::vector<Tuple>& tuples) {
  unsigned i = 0;
  std::ofstream output_file;
  output_file.open(output_file_path.c_str(), std::ofstream::binary);
  while (i < tuples.size()) {
    output_file.write(reinterpret_cast<const char*>(&tuples[i]),
                      4 * sizeof(unsigned));
    i++;
  }
}

static std::string AssembleFilePath(std::string file_path, unsigned file_id) {
  char file_suffix[16];
  sprintf(file_suffix, "%u", file_id);
  file_path += file_suffix;
  return file_path;
}

Tuple* FileManager::GetNextTuple(unsigned file_id) {
  // Lazy input file initialization.
  std::string file_path = AssembleFilePath(this->file_prefix_, file_id);
  if (input_files_[file_id] == NULL) {
    std::cerr << "Opening file " << file_path << std::endl;
    input_files_[file_id] = new std::ifstream();
    input_files_[file_id]->open(file_path.c_str(), std::ifstream::binary);
  }

  if (!input_files_[file_id]->is_open()) {
    std::cerr << "File " << file_path << " has no more tuples" << std::endl;
    return NULL;
  }

  Tuple* new_tuple = GetTuple(input_files_[file_id]);
  new_tuple->tuple_file_id = file_id;

  // Check if file is done and close it.
  if (input_files_[file_id]->peek() == EOF || input_files_[file_id]->eof()) {
    input_files_[file_id]->close();
  }

  std::cerr << "Retrieved tuple " << new_tuple->ToString() << std::endl;
  return new_tuple;
}

void FileManager::CacheTupleToBuffer(Tuple* tuple) {
  output_buffer_.push_back(*tuple);
}

void FileManager::UnformattedWrite(Tuple* tuple) {
  if (tuple != NULL) {
    output_file_.write(reinterpret_cast<const char*>(tuple),
                       4 * sizeof(unsigned));
  }
}

void FileManager::Flush() {
  Tuple* tuple = &output_buffer_[0];
  unsigned size = output_buffer_.size();
  // Term id of the current buffer objects.
  output_file_.write(reinterpret_cast<const char*>(&tuple->term),
                     sizeof(unsigned));
  // Number of entries fo the current term.
  output_file_.write(reinterpret_cast<const char*>(&size), sizeof(unsigned));
  unsigned i = 0;
  while (i < size) {
    tuple = &output_buffer_[i];
    std::cerr << "Writting tuple" << std::endl << tuple->ToString()
              << std::endl;
    // Document id.
    output_file_.write(reinterpret_cast<const char*>(&tuple->document),
                       sizeof(unsigned));
    // Frequency on the current document.
    output_file_.write(reinterpret_cast<const char*>(&tuple->frequency),
                       sizeof(unsigned));
    // Position on the current document.
    output_file_.write(reinterpret_cast<const char*>(&tuple->position),
                       sizeof(unsigned));
    ++i;
  }
  output_file_.flush();
}

void FileManager::InitializeHeap(std::priority_queue<Tuple>* heap) {
  unsigned file_id = 0;
  while (this->input_files_.size()) {
    Tuple* tuple = this->GetNextTuple(file_id);
    if (tuple != NULL) {
      heap->push(*tuple);
      delete tuple;
    }
    file_id++;
  }
}

void FileManager::Split(const std::string& file_path) {
  unsigned long long tuple_count = 0;
  unsigned file_id = 0;
  std::ifstream tuple_file;

  // Determine the amount of tuples in the file pointed by |file_path|.
  tuple_file.open(file_path.c_str(),
                  std::ifstream::binary | std::ifstream::ate);
  tuple_count = tuple_file.tellg() >> 2;  // divide by 4.
  tuple_file.close();

  // Open |tuple_file| for effective input.
  tuple_file.open(file_path.c_str(), std::ifstream::binary);

  while (tuple_count) {
    unsigned tuple_block_size = this->input_files_.size();
    std::vector<Tuple> tuples;

    while (tuple_block_size && tuple_count) {
      tuples.push_back(*(GetTuple(&tuple_file)));
      tuple_block_size--;
      tuple_count--;
    }

    std::sort(tuples.begin(), tuples.end());
    OutputTuples(AssembleFilePath(this->file_prefix_, ++file_id), tuples);
  }

  tuple_file.close();
}

}  // namespace util
