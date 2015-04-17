#include "../query_processor.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <set>
#include <memory>

using namespace std;

int main(int argc, char** argv) {
  if (argc < 3) {
    cout << argv[0] << " [binary vocabulary] [binary index file]" << endl;
    return 0;
  }

  components::QueryProcessor processor(argv[2]);
  processor.BuildBridge(argv[1]);


  while(true) {
    std::string query = "";
    cout << "Waiting query" << endl;
    std::getline(cin, query);
    if(cin.eof()) {
      break;
    }
    std::set<unsigned> answer = processor.QueryIndex(query);
    if(answer.empty()) {
      cout << "One of your terms doesn't match the vocabulary" << endl;
      continue;
    }
    auto i = answer.begin();
    while(i != answer.end()) {
      cout << *i << ' ';
      ++i;
    }
    cout << endl;
  }
  cout << "Goodbye" << endl;
}
