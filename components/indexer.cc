#include "../util/file_manager.h"
#include "../util/vocabulary.h"
#include "../external_sorter/external_sorter.h"
#include <string>
#include <cstdlib>

// TODO(me): enable parameter controling. 

int main(int argc, char** argv) {
  unsigned run_count = 0;
  std::string run_file_path_prefix, output_index_file_path, great_tuple_file_path;
  util::FileManager* file_manager = new util::FileManager(run_count, run_file_path_prefix, output_index_file_path);
  
  // Generates the intermediate sorted run files.
  file_manager->Split(great_tuple_file_path);
  
  // Sort all tuples and produce the inverted index.
  sorting::ExternalSorter external_sorter(file_manager);
  external_sorter.MultiwayMerge();
}
