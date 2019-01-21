#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include "InvertedIndexBuilder.h"
#include "InvertedIndex.h"
#include "InvertedList.h"
#include "BufferedList.h"

#include <map>
#include <set>

int main(int argc, char* argv[]){
	
	if(argc!=3){
		cout<<"Modo de Uso:\n>./generar_docsize entrada_indice_binario archivo_docsize\n";
		return 0;
	}
	
	const char *entrada_indice_binario = argv[1];
	const char *archivo_docsize = argv[2];
	
	InvertedIndex indice;
	indice.load(entrada_indice_binario);
	
	string term;
	InvertedList *lista = NULL;
	BufferedList *buff_list = NULL;
	
	unsigned int max_lista = 0;
	
	unsigned int buff_size = 256;
	unsigned int *buffer_lista = new unsigned int[buff_size];
	unsigned int *buffer_payloads = new unsigned int[buff_size];
	
	cout<<"Realizando preproceso...\n";
	map<string, InvertedList*> mapa = indice.getMap();
	
	map<unsigned int, unsigned int> doc_size;
	map<unsigned int, unsigned int> doc_size_unicos;
	map<unsigned int, unsigned int>::iterator it_docs;
	unsigned docid, payload;
	
	for( auto it_mapa : mapa ){
		lista = it_mapa.second;
		if(lista->size() > max_lista){
			max_lista = lista->size();
		}
		
		memset(buffer_lista, 0, buff_size*sizeof(int));
		memset(buffer_payloads, 0, buff_size*sizeof(int));
				
		if( (buff_list = dynamic_cast<BufferedList*>(lista)) != NULL ){
			buff_list->setBufferMemory(buffer_lista);
			buff_list = NULL;
		}
		if( (buff_list = dynamic_cast<BufferedList*>(lista->getPayloadList())) != NULL ){
			buff_list->setBufferMemory(buffer_payloads);
			buff_list = NULL;
		}
		
		for(lista->reset(); (docid=lista->getDocId())!=END_OF_LIST; lista->next()){
			payload = lista->getPayload();
			doc_size[docid] += payload;
			doc_size_unicos[docid] ++;
		}
		
	}
	
	cout<<"Guardando docsize...\n";
	fstream escritor(archivo_docsize, fstream::trunc | fstream::out);
	
	for( auto it_docs : doc_size ){
		docid = it_docs.first;
		escritor << docid << "\t";
		escritor << doc_size[docid] << "\n";
	}
	escritor.close();
	
	delete [] buffer_lista;
	delete [] buffer_payloads;
	
	
}













