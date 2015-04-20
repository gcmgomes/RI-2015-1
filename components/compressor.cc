#include "compressor.h"

namespace components {

void Compressor::Encode(std::vector<unsigned>& numbers, std::string& encoding) {
  unsigned i = numbers.size() - 1;
  // Compress distances instead of positions.
  while (i) {
    numbers[i] -= numbers[i - 1];
    i--;
  }
  i = 0;
  numbers[i]++;
  std::string initial_encoding = "";
  // Generate a string of 0s and 1s.
  while (i < numbers.size()) {
    EliasGammaEncoding(numbers[i], initial_encoding);
    i++;
  }
  i = 0;
  encoding.clear();
  while (i < initial_encoding.size()) {
    unsigned bits_to_write = 8;
    char byte = 0x0;
    while (i < initial_encoding.size() && bits_to_write) {
      byte <<= 1;
      if (initial_encoding[i] == '1') {
        byte ^= 0x01;
      }
      i++;
      bits_to_write--;
    }
    byte <<= bits_to_write;
    encoding += byte;
  }
  encoding.shrink_to_fit();
}

void Compressor::Decode(unsigned numbers_count, std::string& encoding,
                        std::vector<unsigned>& numbers) {
  std::string initial_decoding = "";
  unsigned i = 0;
  while (i < encoding.size()) {
    unsigned bits_decoded = 0;
    while (bits_decoded < 8) {
      if (encoding[i] & 0x1000) {
        initial_decoding += '1';
      } else {
        initial_decoding += '0';
      }
      encoding[i] <<= 1;
      bits_decoded++;
    }
    i++;
  }
  unsigned pos = 0;
  i = 0;
  while (i < numbers_count) {
    numbers.push_back(EliasGammaDecoding(pos, initial_decoding));
    if (i) {
      numbers[i] += numbers[i - 1];
    } else {
      numbers[i]--;
    }
    i++;
  }
}

void Compressor::EliasGammaEncoding(unsigned number, std::string& encoding) {
  if (number == 1) {
    encoding += '1';
    return;
  }

  unsigned N = 8 * sizeof(unsigned) - 1, highest_bit_mask = 0x80000000;
  unsigned temp = number;
  // Let N be a value such that 2^N <= |number| < 2^(N+1).
  while (!(temp & highest_bit_mask)) {
    temp <<= 1;
    N--;
  }
  // Write out N 0 bits.
  unsigned i = 0;
  while (i < N) {
    encoding += '0';
    i++;
  }
  // Append the N+1 binary form of |number|.
  N++;
  while (N) {
    if (temp & highest_bit_mask) {
      encoding += '1';
    } else {
      encoding += '0';
    }
    N--;
    temp <<= 1;
  }
}

unsigned Compressor::EliasGammaDecoding(unsigned& starting_pos,
                                        std::string& encoding) {
  int N = 0;
  unsigned number = 0;
  // Read and count 0s until the first one is reached.
  while (encoding[N + starting_pos] == '0') {
    N++;
  }
  unsigned i = N;
  while (N >= 0) {
    if (encoding[i + starting_pos] == '1') {
      number ^= 0x00000001;
    }
    if (N > 0) {
      number <<= 1;
    }
    N--;
    i++;
  }
  starting_pos += i;
  return number;
}

}  // namespace components
