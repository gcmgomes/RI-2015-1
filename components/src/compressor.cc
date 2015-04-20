#include "../compressor.h"
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  if(argc < 2) {
    cout << argv[0] << " [number to be encoded]" << endl;
    return 0;
  }
  string encoding;
  vector<unsigned> numbers;
  unsigned i = 1;
  while(i < (unsigned) argc) {
    unsigned number = 0;
    sscanf(argv[i], "%u", &number);
    numbers.push_back(number);
    i++;
  }
  i--;
  components::Compressor c;
  c.Encode(numbers, encoding);
  cout << "Original size: " << numbers.size() * sizeof(unsigned) * 8<< endl;
  cout << "Encoded size:  " << encoding.size() * sizeof(char) * 8 << endl;
  numbers.clear();
  c.Decode(i, encoding, numbers);
  i = 0;
  while(i < numbers.size()) {
    cout << numbers[i] << ' ';
    i++;
  }
  cout << endl;
}
