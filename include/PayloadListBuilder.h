#if !defined(_PAYLOAD_LIST_BUILDER_H)
#define _PAYLOAD_LIST_BUILDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>

#include <iostream>
#include <fstream>
#include <sstream>

#include <list>

#include "PayloadList.h"
#include "DummyPayloadList.h"
#include "PForDeltaPayloadList.h"
//#include "Simple9PayloadList.h"

#include "MinPayloadList.h"

//#include "PForDeltaPayloadListCover.h"

using namespace std;

class PayloadListBuilder{

private:
	unsigned int buff_size;
	unsigned int *buff;

protected:
	
public:
	
	PayloadListBuilder();
	
	virtual ~PayloadListBuilder();
	
	virtual void saveList(PayloadList *payload_list, unsigned int max_payload, fstream *writer);
	
	virtual PayloadList *loadList(fstream *reader, unsigned int &max_payload);
	
	virtual PayloadList *buildList(unsigned char type, list<unsigned int> *original_list);
	
	virtual PayloadList *buildList(unsigned char type, unsigned int *arr, unsigned int arrsize);
	
//	virtual PayloadList *getCover(PayloadList *original);
	
};

#endif

