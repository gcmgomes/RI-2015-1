#include "sorter.h"

namespace sorting {

Sorter::Sorter(util::FileManager* file_manager) {
  file_manager_ = file_manager;
}

Sorter::~Sorter() {
  if (file_manager_ != NULL) {
    delete file_manager_;
  }
}

void Sorter::ExternalMultiwayMerge() {
  file_manager_->InitializeHeap(&(this->heap_));
  unsigned sorted_tuples = heap_.size() - 1;
  const util::Tuple* tuple = nullptr;

  while (!heap_.empty()) {
    tuple = &heap_.top();

    file_manager_->WriteTuple(tuple);
    sorted_tuples++;
    if (!(sorted_tuples % 1000)) {
      std::cout << "\rSorted tuples: " << sorted_tuples;
      std::fflush(stdout);
    }
    tuple = file_manager_->GetNextTuple(tuple->tuple_file_id);
    heap_.pop();

    if (tuple != NULL) {
      heap_.push(*tuple);
      delete tuple;
    }
  }

  std::cout << std::endl;
  file_manager_->Flush();
  file_manager_->CloseOutput();
}

}  // namespace sorting
