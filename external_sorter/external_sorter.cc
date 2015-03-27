#include "../util/tuple.h"

namespace sorting {

ExternalSorter::ExternalSorter(util::FileManager* file_manager) {
 file_manager_ = file_manager;
}

~ExternalSorter() {
 if(file_manager_ != NULL;) {
  delete file_manager_;
 }
}

void ExternalSorting::MultiwayMerge() {
 heap_.clear();
 file_manager_->InitializeHeap(&(this->heap_));
 util::Tuple* tuple = NULL;
 
 while(heap_.empty() == false) {
  tuple = &(heap_.top());
  file_manager_->OutputTuple(tuple);
  
  tuple = file_manager_->GetNextTuple(tuple);
  heap_.pop();
  
  if(tuple != NULL) {
   heap_.push(*tuple);
  }
 }
}

} // namespace sorting
