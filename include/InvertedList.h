#if !defined(_INVERTED_LIST_H)
#define _INVERTED_LIST_H

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <list>

#include "PayloadList.h"

#define END_OF_LIST 0xffffffff

using namespace std;

//Lista de deltas, debe encargarse de mantener y responder docids absolutos
class InvertedList{

private:

protected:
	//solo para debug, no lo necesita para funcionar
	unsigned int term_id;
	
	PayloadList *payload_list;
	//una opcion es que la variable siguiente sea establecida como la posicion del doc actual
	//de este modo, los hijos no tendrian que reimplementar los metodos de payloads
	//solo asegurar que cur_pos siempre mantenga la posicion correcta
	unsigned int cur_pos;
	unsigned int max_payload;
	
public:

	InvertedList();
	InvertedList(list<unsigned int> *original_list);
	InvertedList(unsigned int *arr, unsigned int arr_size);
	virtual ~InvertedList();
	
	//Retorna la lista a su estado inicial, en el primer doc
	//notar que una lista puede no tener next, y aun estar en una posicion valida (la ultima)
	virtual void reset();
	
	//Retorna true si existe un documento luego del actual
	virtual bool hasNext();
	
	//Avanza una posicion y retorna el nuevo doc
	//Si pasa el ultimo doc de la lista, retorna END_OF_LIST
	//Si ya esta en una posicion invalida, no avanza y retorna END_OF_LIST
	virtual unsigned int next();
	
	//Avanza hasta el primer doc mayor o igual a desired, y lo retorna
	//Si el doc actual es mayor o igual a desired, no avanza y retorna el doc actual
	//Si desired es mayor al ultimo doc de la lista, esta queda en la posicion inválida y retorna END_OF_LIST
	//(eso es equivalente a realizar un next() estando en el ultimo doc)
	virtual unsigned int next(unsigned int desired);
	
	//Retorna el numero de docs
	virtual unsigned int size() const;
	
	//Retorna el doc (absoluto) actual, o END_OF_LIST si esta en una posicion invalida
	virtual unsigned int getDocId() const;
	
	//Descomprime todos los docs en el buff
	//Recibe el buff con memoria pedida, y asume que el llamador asegura su tamaño
	virtual unsigned int uncompress(unsigned int *buff);
	
	//Serializa y escribe en binario la lista
	//No guarda el estado o la posicion actual, la recarga siempre parte desde el primer doc
	virtual void save(fstream *writer);
	
	//Recarga la lista (la deja en la posicion inicial)
	virtual void load(fstream *reader);
	
	//retorna el numero de bytes para almacenar los datos
	//No se incluye el numero de docs, ni el tipo de compresion, ni la estructura de bloques
	virtual unsigned int getSizeData() const;
	
	//retorna el numero de bytes para almacenar la estructura de bloques
	//se incluye todo lo necesario para la estructura de bloques (incluido el tamaño de la estructura)
	virtual unsigned int getSizeBlock() const;
	
	//Este metodo es solo para debug
	//En la practica, la lista no necesita conocer su term_id
	void setTermId(unsigned int _term_id);
	unsigned int getTermId() const;
	
	//Interface para obtener Payload (por ahora lo dejo uint)
	virtual unsigned int getPayload() const;
	
	//El metodo siguiente parece muy especifico para Wand
	//Quizas sea mejor definir uno para retorna algun dato global de la lista no especificado
	//En ese caso, el dato tambien tendria que leerse desde el texto como en rise
	virtual unsigned int getMaxPayload() const;
	
	void setPayloadList(PayloadList *_payload_list, unsigned int _max_payload);
	PayloadList * getPayloadList();
	
	
};

#endif

