#include "../page_url.h"

using namespace std;

int main(int argc, char** argv) {
  if(argc < 2) {
    cout << argv[0] << " [url] [relative url]" << endl;
    return 0;
  }
  util::PageUrl url(argv[1]), url2 = url;
  cout << url.url() << endl;
}
