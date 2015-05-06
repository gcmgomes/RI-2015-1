#include "../page_url.h"

using namespace std;

int main(int argc, char** argv) {
  if(argc < 3) {
    cout << argv[0] << " [url] [relative url]" << endl;
    return 0;
  }
  util::PageUrl url(argv[1]), url2 = url;
  url2.RelativeMove(argv[2]);
  cout << url.url() << endl << url2.url() << endl;
  cout << endl;
}
