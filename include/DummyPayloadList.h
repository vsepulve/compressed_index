#ifndef _DUMMY_PAYLOAD_LIST_H
#define _DUMMY_PAYLOAD_LIST_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <list>

#include "PayloadList.h"

using namespace std;

class DummyPayloadList : public PayloadList{

protected:
	unsigned int n_payloads;
	unsigned char *payloads;

public:
	//la lista vacia quizas deba ser dejada private para que no sea usada
	DummyPayloadList();
	DummyPayloadList(list<unsigned int> *original_list);
	DummyPayloadList(unsigned int *arr, unsigned int arr_size);
	~DummyPayloadList();
	
	//por el momento, este modulo es de acceso realmente aleatorio
	//por lo que el reset no es necesario
	void reset();
	
	unsigned int getPayload(unsigned int pos);
	
	unsigned int size() const;
	
	unsigned int getSizeData() const;
	
	unsigned int getSizeBlock() const;
	
	//Serializa y escribe en binario la lista
	//No guarda el estado o la posicion actual, la recarga siempre parte desde el primer payload
	void save(fstream *writer);
	
	//Recarga la lista (la deja en la posicion inicial)
	void load(fstream *reader);
	
};

#endif //RESULTOBJECT_H
