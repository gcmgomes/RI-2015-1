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
//  unsigned sorted_tuples = 0;

  while (!heap_.empty()) {
    file_manager_->WriteTuple(heap_.top().get());

/*    sorted_tuples++;
    if (!(sorted_tuples % 100)) {
      std::cout << "\rSorted tuples: " << sorted_tuples;
      std::fflush(stdout);
    }*/

    std::unique_ptr<util::Tuple> tuple =
        file_manager_->GetNextTuple(heap_.top()->tuple_file_id);
    heap_.pop();

    if (tuple != NULL) {
      heap_.push(std::move(tuple));
    }
  }

//  std::cout << "\rSorted tuples: " << sorted_tuples << std::endl;
  file_manager_->Flush();
  file_manager_->CloseOutput();
}

}  // namespace sorting
