#if !defined(_INVERTED_INDEX_H)
#define _INVERTED_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <set>
#include <list>
#include <map>

#include "InvertedList.h"
#include "InvertedListBuilder.h"
#include "PayloadListBuilder.h"

#define MAX_WORD 1024

using namespace std;

class InvertedIndex{

protected:
	map<string, InvertedList*> mapa;
	
	// Innecesario, la deberia ser estatico
	InvertedListBuilder list_builder;
	PayloadListBuilder payload_list_builder;
//	map<unsigned int, InvertedList*>::iterator it_global;
	
public:
	
	InvertedIndex();
	
	virtual ~InvertedIndex();
	
	map<string, InvertedList*> &getMap();
	
	virtual bool load(const char *index_file);
	
//	virtual bool load(const char *index_file, const char *terms_file);
	
	virtual void save(const char *index_file);
	
	virtual InvertedList *getList(string &term);
	
//	virtual InvertedList *getListCover(unsigned int term_id);
	
	virtual void addList(string &term, InvertedList *inverted_list);
	
	virtual unsigned int size();
	
	//Metodos para iterar por las listas del indice
//	virtual void reset();
//	virtual InvertedList *getCurList();
//	virtual unsigned int getCurTerm();
//	virtual InvertedList *next();
	
};

#endif

