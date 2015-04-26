#ifndef _RI_2015_1_COMPONENTS_COMPRESSOR_H_
#define _RI_2015_1_COMPONENTS_COMPRESSOR_H_

#include <iostream>
#include <string>
#include <vector>

namespace components {

class Compressor {
 public:
  void Encode(const std::vector<unsigned>& positions, std::string& encoding);
  void Decode(unsigned numbers_count, std::string& encoding,
              std::vector<unsigned>& numbers);

 private:
  void EliasGammaEncoding(unsigned number, std::string& encoding);
  unsigned EliasGammaDecoding(unsigned& starting_pos, std::string& encoding);
};
}  // namespace components

#endif
