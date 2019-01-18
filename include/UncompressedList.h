#if !defined(_UNCOMPRESSED_LIST_H)
#define _UNCOMPRESSED_LIST_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <list>

#include "InvertedList.h"

using namespace std;

class UncompressedList : public InvertedList{

private:

protected:
	unsigned int n_docs;
	unsigned int *docs;
	//cur_pos esta definido en InvertedList
//	unsigned int cur_pos;
	
public:

	UncompressedList();
	UncompressedList(list<unsigned int> *original_list);
	UncompressedList(unsigned int *arr, unsigned int arr_size);
	virtual ~UncompressedList();
	
	//deja la lista en el primer documento
	//notar que una lista puede no tener next, y aun estar en una posicion valida (la ultima)
	virtual void reset();
	
	virtual bool hasNext() const;
	
	virtual unsigned int next();

	virtual unsigned int next(unsigned int desired);
	
	virtual unsigned int size() const;
	
	virtual unsigned int getDocId() const;
	
	virtual unsigned int uncompress(unsigned int *buff);
	
	virtual void save(fstream *writer);
	
	virtual void load(fstream *reader);
	
	//retorna el numero de bytes para almacenar los datos
	//No se incluye el numero de docs, ni el tipo de compresion, ni la estructura de bloques
	virtual unsigned int getSizeData() const;
	
	//retorna el numero de bytes para almacenar la estructura de bloques
	//se incluye todo lo necesario para la estructura de bloques (incluido el tamaño de la estructura)
	virtual unsigned int getSizeBlock() const;
	
	//Metodos para entrega de datos directos para la clase Cover
	unsigned int *getData();
	
	
};

#endif

