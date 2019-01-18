#ifndef _MIN_PAYLOAD_LIST_H
#define _MIN_PAYLOAD_LIST_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include <list>

#include "PayloadList.h"

using namespace std;

class MinPayloadList : public PayloadList{

private:
	unsigned char payload;

public:
	//la lista vacia quizas deba ser dejada private para que no sea usada
	MinPayloadList();
	MinPayloadList(list<unsigned int> *original_list);
	MinPayloadList(unsigned int *arr, unsigned int arr_size);
	MinPayloadList(MinPayloadList *original);
	virtual ~MinPayloadList();
	
	//la idea es que el acceso no necesite ser realmente aleatorio, sino creciente
	//de ahi un reset, para luego solo aceptar llamadas a getPayload
	//en que pos sea mayor a la posicion actual
	//esto solo es util si ayuda a la eficiencia
	virtual void reset();
	
	virtual unsigned int getPayload(unsigned int pos);
	
	virtual unsigned int size() const;
	
	virtual unsigned int getSizeData() const;
	
	virtual unsigned int getSizeBlock() const;
	
	//Serializa y escribe en binario la lista
	//No guarda el estado o la posicion actual, la recarga siempre parte desde el primer payload
	virtual void save(fstream *writer);
	
	//Recarga la lista (la deja en la posicion inicial)
	virtual void load(fstream *reader);
	
};

#endif //RESULTOBJECT_H
