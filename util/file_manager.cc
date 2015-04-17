#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <cmath>
#include "file_manager.h"

using namespace std;
namespace util {

FileManager::FileManager(unsigned file_count, std::string input_path,
                         std::string output_file_path) {
  input_path_ = input_path;
  input_tuples_ = 0;
  unsigned i = 0;
  while (i < file_count) {
    input_files_.push_back(nullptr);
    i++;
  }
  if (!output_file_path.empty()) {
    output_file_.reset(new std::fstream());
    output_file_->open(output_file_path.c_str(),
                       std::fstream::binary | std::fstream::out);
  }
  indexer_.reset(nullptr);
}

FileManager::~FileManager() {
  if (output_file_ != nullptr) {
    output_file_->close();
  }
}

static std::unique_ptr<Tuple> GetTuple(
    std::unique_ptr<std::fstream>& input_file) {
  std::unique_ptr<Tuple> new_tuple(new Tuple(0, 0, 0, 0));
  input_file->read(reinterpret_cast<char*>(new_tuple.get()),
                   4 * sizeof(unsigned));
  return new_tuple;
}

std::unique_ptr<Tuple> FileManager::GetNextTuple(unsigned file_id) {
  // Lazy input file initialization.
  if (input_files_[file_id] == nullptr) {
    InputFileInitialization(file_id);
  }

  if (!input_files_[file_id]->is_open()) {
    return nullptr;
  }

  std::unique_ptr<Tuple> new_tuple = GetTuple(input_files_[file_id]);
  new_tuple->tuple_file_id = file_id;

  // Check if file should be closed.
  if (CloseForInput(file_id)) {
    input_files_[file_id]->close();
  }

  return std::move(new_tuple);
}

static void WriteTupleInternal(const Tuple* tuple,
                               std::unique_ptr<std::fstream>& file) {
  if (tuple != NULL && file->is_open()) {
    file->write(reinterpret_cast<const char*>(tuple), 4 * sizeof(unsigned));
  }
}

void FileManager::WriteTuple(const Tuple* tuple) {
  WriteTupleInternal(tuple, output_file_);
}

void FileManager::Index(const Tuple* tuple) {
  if(indexer_ == nullptr) {
    indexer_.reset(new components::Indexer());
  }
  indexer_->WriteTuple(tuple, output_file_);
}

void FileManager::FinishIndex() {
  if (indexer_ != nullptr) {
    indexer_->FinishIndex(output_file_);
  }
}

void FileManager::Flush() {
  if (output_file_->is_open()) {
    output_file_->flush();
  }
}

void FileManager::CloseOutput() {
  if (output_file_->is_open()) {
    output_file_->close();
  }
}

void FileManager::InitializeHeap(std::priority_queue<
    std::unique_ptr<Tuple>, std::vector<std::unique_ptr<Tuple> >, TupleCompare>*
                                     heap) {
  unsigned file_id = 0;
  while (file_id < this->input_files_.size()) {
    std::unique_ptr<Tuple> tuple = this->GetNextTuple(file_id);
    if (tuple != nullptr) {
      heap->push(std::move(tuple));
    }
    file_id++;
  }
}

void FileManager::PartialEmplaceSort() {
  std::vector<Tuple> tuples;
  unsigned file_id = 0;
  while (file_id < input_files_.size()) {
    tuples.clear();

    do {
      tuples.push_back(*GetNextTuple(file_id).get());

    } while (!CloseForInput(file_id));
    InputFileInitialization(file_id);

    std::sort(tuples.begin(), tuples.end());

    unsigned i = 0;
    while (i < tuples.size()) {
      WriteTupleInternal(&tuples[i], input_files_[file_id]);
      i++;
    }
    input_files_[file_id]->close();
    file_id++;
  }
}

static const std::string GetTupleText(std::fstream* input_file) {
  Tuple tuple(0, 0, 0, 0);
  input_file->read(reinterpret_cast<char*>(&tuple), 4 * sizeof(unsigned));
  return tuple.ToString();
}

void FileManager::ConvertBinToText(const std::string& binary_input_file_path,
                                   const std::string& text_output_file_path) {
  std::fstream input_file(binary_input_file_path.c_str(),
                          std::fstream::binary | std::fstream::in);
  std::fstream output_file(text_output_file_path.c_str(), std::fstream::out);

  std::string str = GetTupleText(&input_file);
  while (!input_file.eof() && input_file.peek() != EOF) {
    output_file << str << std::endl;
    str = GetTupleText(&input_file);
  }

  output_file << str << std::endl;
  input_file.close();
  output_file.close();
}

unsigned FileManager::CountTuples(const std::string& input_file_path) {
  std::fstream tuple_file;
  tuple_file.open(input_file_path.c_str(),
                  std::fstream::binary | std::fstream::in);
  // Move pointer to EOF.
  tuple_file.seekg(0, tuple_file.end);
  unsigned val = tuple_file.tellg() / (4 * sizeof(unsigned));
  tuple_file.close();
  return val;
}

void FileManager::InputFileInitialization(unsigned file_id) {
  if (input_tuples_ == 0) {
    input_tuples_ = CountTuples(input_path_);
    input_tuples_per_block_ = (unsigned)ceil((double)input_tuples_ /
                                             (double)this->input_files_.size());
  }

  std::unique_ptr<std::fstream>& file = input_files_[file_id];
  file.reset(new std::fstream());

  // std::fstream::out needed for emplace sorting.
  file->open(input_path_.c_str(),
             std::fstream::binary | std::fstream::in | std::fstream::out);

  file->seekg(input_tuples_per_block_ * file_id * 4 * sizeof(unsigned));
  file->seekp(file->tellg());
}

bool FileManager::CloseForInput(unsigned file_id) {
  unsigned next_block_start_point =
      input_tuples_per_block_ * (file_id + 1) * 4 * sizeof(unsigned);
  std::unique_ptr<std::fstream>& file = input_files_[file_id];
  return file->peek() == EOF || file->eof() ||
         ((unsigned)file->tellg() == next_block_start_point);
}

}  // namespace util
