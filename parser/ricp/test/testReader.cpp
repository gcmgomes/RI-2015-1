/**
 * Test the CollectionReader class
 * @author: anisio@dcc.ufmg.br
 */

#include "../src/CollectionReader.h"

#include <cstdlib>
#include <iostream>

using namespace std;
using namespace RICPNS;

int main(int argc, char** argv) {

	cout << "Testing CollectionReader class..." << endl;

	string inputDirectory(argv[1]);
	string indexFileName(argv[2]);

	CollectionReader * reader = new CollectionReader(inputDirectory,
													 indexFileName);
	Document doc;
	doc.clear();
	int i = 0;
	while(reader->getNextDocument(doc))	{
		//if(doc.getURL() == "http://www.metodoinvestimentos.com.br/parceiros.asp") {
		//	cout << "[" << doc.getURL() << "] [" << doc.getLength() << "]" << endl;
		//	cout << "BEGINHTML[" << doc.getText() << "]ENDHTML" << endl;
		//}
			cerr << "[" << doc.getURL() << "]" << endl;
		doc.clear();
		++i;
	}
	cerr << "Total [" << i << "]" << endl;

	delete reader;

	cout << "Test finished." << endl;

	return EXIT_SUCCESS;
}

