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
		cout<<"Modo de Uso:\n>./generar_idf entrada_indice_binario archivo_idf\n";
		return 0;
	}
	
	const char *entrada_indice_binario = argv[1];
	const char *archivo_idf = argv[2];
	
	InvertedIndex indice;
	indice.load(entrada_indice_binario);
	
//	unsigned int term_id;
	string term;
	InvertedList *lista = NULL;
	BufferedList *buff_list = NULL;
	
	unsigned int max_lista = 0;
	
	unsigned int buff_size = 256;
	unsigned int *buffer_lista = new unsigned int[buff_size];
	unsigned int *buffer_payloads = new unsigned int[buff_size];
	
	cout<<"Realizando preproceso...\n";
	map<string, InvertedList*> mapa = indice.getMap();
	map<string, double> mapa_idf;
	set<unsigned int> coleccion;
	unsigned docid;
	
	for( auto it_mapa : mapa ){
		term = it_mapa.first;
		lista = it_mapa.second;
		if(lista->size() > max_lista){
			max_lista=lista->size();
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
			coleccion.insert(docid);
		}
		
	}
	
	cout<<"Calculando idf...\n";
	for( auto it_mapa : mapa ){
		term = it_mapa.first;
		lista = it_mapa.second;
		
		//Formula original y mas directa de idf
		mapa_idf[term] = log( ((double)coleccion.size())/lista->size() );
		
		//Formula de idf para okapi-bm25 (de wikipedia)
//		mapa_idf[term] = log( ((double)coleccion.size() - lista->size() + 0.5)/ (lista->size() + 0.5) );
		
//		if(term_id<10){
//			cout<<"mapa_idf["<<term_id<<"]: "<<mapa_idf[term_id]<<"\n";
//		}
	}
	
	cout<<"Guardando idf...\n";
	fstream escritor(archivo_idf, fstream::trunc | fstream::out);
	for( auto it_mapa : mapa ){
		term = it_mapa.first;
		escritor << term << "\t";
		escritor << mapa_idf[term] << "\n";
	}
	escritor.close();
	
	delete [] buffer_lista;
	delete [] buffer_payloads;
	
}













