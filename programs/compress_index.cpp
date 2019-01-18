#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#include <algorithm>

#include <map>
#include <vector>
#include <list>

#include "InvertedIndex.h"
#include "InvertedIndexBuilder.h"

using namespace std;

int main(int argc, char* argv[]){

	if(argc != 4){
		cout << "\n";
		cout << "Usage: compress_index text_index output_index compression_type\n";
		cout << "List types: 1 (Uncompressed), 2 (VarByte), 3 (Simple9), 4 (PForDelta)\n";
		cout << "For now payloads are always compressed with PForDelta\n";
		cout << "\n";
		return 0;
	}
	
	const string input = argv[1];
	const string output = argv[2];
	unsigned int comp_type = atoi(argv[3]);
	
	cout << "Start (from \"" << input << "\", to \"" << output << "\", using type " << comp_type << ")\n";
	
	// Text input, compression type, payloads type, renumeration map
	InvertedIndex *index = InvertedIndexBuilder::createIndex(input, comp_type, 2, NULL);
	index->save(output);
	delete index;
	
	cout << "End\n";
	
}

















