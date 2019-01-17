#if !defined(_VARBYTE_LIST_H)
#define _VARBYTE_LIST_H

#include <stdlib.h>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <list>
//estructuras creadas
#include "InvertedList.h"
#define DOCS_PER_BLOCK 128

using namespace std;

class VarByteList : public InvertedList{

protected:

	unsigned int n_docs;
	unsigned char *data;
	
	//datos para el next
	unsigned int cur_index;
	unsigned int cur_doc;
	unsigned int data_size;
	
	//datos para la info de bloques
	unsigned int num_blocks;
	unsigned int cur_block_pos;
	unsigned long long *bloks;
	
	unsigned int block_pos(unsigned int desired);
	unsigned int nbytes(unsigned int num);
	
public:

	VarByteList();
	VarByteList(list<unsigned int> *original_list);
	VarByteList(unsigned int *arr, unsigned int arr_size);
	virtual ~VarByteList();
	
	//deja la lista en el primer documento
	//notar que una lista puede no tener next, y aun estar en una posicion valida (la ultima)
	virtual void compress(unsigned int *arr, unsigned int arr_size);
	
	virtual void reset();
	
	virtual bool hasNext();
	
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
	unsigned char *getData(){
		return data;
	}
	unsigned int getDataSize(){
		return data_size;
	}
	unsigned long long *getBlocks(){
		return bloks;
	}
	unsigned int getNumBlocks(){
		return num_blocks;
	}
	
};

#endif
