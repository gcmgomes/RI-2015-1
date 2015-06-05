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
  unsigned long long sorted_tuples = 0;
  while (!heap_.empty()) {
    file_manager_->Index(heap_.top().get());

    std::unique_ptr<util::Tuple> tuple =
        file_manager_->GetNextTuple(heap_.top()->tuple_file_id);
    sorted_tuples++;
    if((sorted_tuples % 1000) == 0) {
      std::cout << '\r' << sorted_tuples;
      fflush(stdout);
    }
    heap_.pop();

    if (tuple != NULL) {
      heap_.push(std::move(tuple));
    }
  }

  file_manager_->FinishIndex();
  file_manager_->Flush();
  file_manager_->CloseOutput();
}

}  // namespace sorting
