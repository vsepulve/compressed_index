#if !defined(_INVERTED_INDEX_BUILDER_H)
#define _INVERTED_INDEX_BUILDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <vector>

#include "InvertedList.h"
#include "InvertedIndex.h"
#include "InvertedListBuilder.h"
#include "PayloadList.h"
#include "PayloadListBuilder.h"
#include "BufferedList.h"

using namespace std;

class InvertedIndexBuilder{

protected:
	static void loadRenumerationTable(const string &renum_file, map<unsigned int, unsigned int> *renumeration);
	
public:

	InvertedIndexBuilder();
	
	~InvertedIndexBuilder();
	
	// carga un indice en texto
	// Lee listas de pares docid y payload, y los ordena por docid
	// Luego convierte los docid a deltas
	// El formato esperado por cada linea de text_file es:
	// "term n_docs doc1 payload1 doc2 payload2... \n"
	// Si se incluye una tabla de renumeracion, reemplaza los docids por sus versiones ajustadas antes de oredenar
	static InvertedIndex *createIndex(const string &text_file, unsigned char list_type, unsigned char payload_type, map<unsigned int, unsigned int> *renumeration);
	
	// Variantes para comodidad
	static InvertedIndex *createIndex(const string &text_file, unsigned char list_type);
	static InvertedIndex *createIndex(const string &text_file, unsigned char list_type, unsigned char payload_type);
	static InvertedIndex *createIndex(const string &text_file, unsigned char list_type, const string &renum_file);
	static InvertedIndex *createIndex(const string &text_file, unsigned char list_type, unsigned char payload_type, const string &renum_file);
	
	
};

#endif

