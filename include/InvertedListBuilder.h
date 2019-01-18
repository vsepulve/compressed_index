#if !defined(_INVERTED_LIST_BUILDER_H)
#define _INVERTED_LIST_BUILDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>

#include <iostream>
#include <fstream>
#include <sstream>

#include <list>

#include "InvertedList.h"
#include "UncompressedList.h"
#include "VarByteList.h"
//#include "RLVarByteList.h"
#include "Simple9.h"
//#include "RLSimple9.h"
//#include "interpolative.h"
//#include "RLPForDelta.h"
#include "PForDelta.h"

#include "MinInvertedList.h"

//#include "UncompressedListCover.h"
//#include "VarByteListCover.h"

using namespace std;

class InvertedListBuilder{

private:
	unsigned int buff_size;
	unsigned int *buff;

protected:
	
public:
	
	InvertedListBuilder();
	
	virtual ~InvertedListBuilder();
	
	virtual void saveList(InvertedList *inverted_list, fstream *writer);
	
	virtual InvertedList *loadList(fstream *reader);
	
	virtual InvertedList *buildList(unsigned char type, list<unsigned int> *original_list);
	
	virtual InvertedList *buildList(unsigned char type, unsigned int * arr, unsigned int arrsize);
	
//	virtual InvertedList *getCover(InvertedList *original);
	
};

#endif

