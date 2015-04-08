#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cmath>
#include "file_manager.h"

namespace util {

FileManager::FileManager(unsigned file_count, std::string file_prefix,
                         std::string output_file_path) {
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
  Tuple* new_tuple = new Tuple(0,0,0,0);
  input_file->read(reinterpret_cast<char*>(new_tuple), 4 * sizeof(unsigned));
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
    input_files_[file_id] = new std::ifstream();
    input_files_[file_id]->open(file_path.c_str(), std::ifstream::binary);
  }

  if (!input_files_[file_id]->is_open()) {
    return NULL;
  }

  Tuple* new_tuple = GetTuple(input_files_[file_id]);
  new_tuple->tuple_file_id = file_id;
  // Check if file is done and close it.
  if (input_files_[file_id]->peek() == EOF || input_files_[file_id]->eof()) {
    input_files_[file_id]->close();
  }

  return new_tuple;
}

void FileManager::WriteTuple(const Tuple* tuple) {
  if (tuple != NULL && output_file_.is_open()) {
    output_file_.write(reinterpret_cast<const char*>(tuple),
                       4 * sizeof(unsigned));
  }
}

void FileManager::Flush() {
  if (output_file_.is_open()) {
    output_file_.flush();
  }
}

void FileManager::CloseOutput() {
  if (output_file_.is_open()) {
    output_file_.close();
  }
}

void FileManager::InitializeHeap(std::priority_queue<
    util::Tuple, std::vector<util::Tuple>, util::TupleCompare>* heap) {
  unsigned file_id = 0;
  while (file_id < this->input_files_.size()) {
    Tuple* tuple = this->GetNextTuple(file_id);
    if (tuple != NULL) {
      heap->push(*tuple);
      delete tuple;
    }
    file_id++;
  }
}

void FileManager::Split(const std::string& file_path) {
  unsigned file_id = 0;
  std::ifstream tuple_file;

  // Determine the amount of tuples in the file pointed by |file_path|.
  tuple_file.open(file_path.c_str(), std::ifstream::binary);
  tuple_file.seekg(0, tuple_file.end);

  unsigned long long tuple_count = 0;
  tuple_count = tuple_file.tellg() / (4 * sizeof(unsigned));

  // Set |tuple_file| for effective input.
  tuple_file.seekg(0, tuple_file.beg);

  unsigned tuple_block_size =
      (unsigned)ceil((double)tuple_count / (double)this->input_files_.size());

  while (tuple_count) {
    int current_block_size = tuple_block_size;
    std::vector<Tuple> tuples;

    while (current_block_size && tuple_count) {
      Tuple* tuple = GetTuple(&tuple_file);
      tuples.push_back(*tuple);
      delete tuple;
      current_block_size--;
      tuple_count--;
    }

    std::sort(tuples.begin(), tuples.end());
    OutputTuples(AssembleFilePath(this->file_prefix_, file_id++), tuples);
  }

  tuple_file.close();
}

}  // namespace util
