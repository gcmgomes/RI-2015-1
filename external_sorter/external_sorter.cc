#include "external_sorter.h"

namespace sorting {

ExternalSorter::ExternalSorter(util::FileManager* file_manager) {
 file_manager_ = file_manager;
}

~ExternalSorter() {
 if(file_manager_ != NULL) {
  delete file_manager_;
 }
}

void ExternalSorting::MultiwayMerge() {
 heap_.clear();
 file_manager_->InitializeHeap(&(this->heap_));
 util::Tuple* tuple = heap_.top();
 unsigned previous_term_id = tuple->term;
 
 while(!heap_.empty()) {
  tuple = &(heap_.top());
  
  if(tuple->term != previous_term_id) {
   previous_term_id = tuple->term;
   file_manager_->Flush();
  }
  
  file_manager_->CacheTupleToBuffer(tuple);
  tuple = file_manager_->GetNextTuple(tuple.tuple_file_id);
  heap_.pop();
  
  if(tuple != NULL) {
   heap_.push(*tuple);
  }
 }
 file_manager_->Flush();
}

} // namespace sorting
