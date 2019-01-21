#include <stdio.h>
#include <stdlib.h>

using namespace std;

#include "InvertedIndex.h"
#include "InvertedList.h"
#include "PayloadList.h"
#include "BufferedList.h"

#include <map>
#include <set>

map<unsigned int, unsigned int> *loadDocs(const char *entrada_docs){
	
	map<unsigned int, unsigned int> *res=new map<unsigned int, unsigned int>();
	
	ifstream entrada;
	entrada.open(entrada_docs);
	
	string linea, palabra;
	unsigned int docid, largo;
	
	while(true){
		getline(entrada, linea);
		if(!entrada.good() || linea.size()<1){
			break;
		}
		stringstream toks(linea);
		
		toks>>dec>>docid;
		toks>>dec>>largo;
		
		if(docid!=0 && largo!=0){
			(*res)[docid] = largo;
		}
		
	}
	entrada.close();
	
	return res;
}

map<string, float> *loadIdf(const char *entrada_idf){
	
	map<string, float> *res=new map<string, float>();
	
	ifstream entrada;
	entrada.open(entrada_idf);
	
	string linea, palabra;
	string term;
	double idf;
	
	while(true){
		getline(entrada, linea);
		if(!entrada.good() || linea.size()<1){
			break;
		}
		stringstream toks(linea);
		
		toks >> term;
		toks >> idf;
		
		if( term.length() > 0 && idf != 0 ){
			(*res)[term] = idf;
		}
		
	}
	entrada.close();
	
	return res;
}

inline unsigned int log2bits(unsigned int num){
	unsigned int ret=1;
	while( num>>=1 ){
		++ret;
	}
	return ret;
}

int main(int argc, char* argv[]){
	
	if(argc!=7){
		cout<<"Modo de Uso:\n>./generar_docsize entrada_indice_binario archivo_idf archivo_docs k1 b salida_ubs\n";
		return 0;
	}
	
	const char *entrada_indice_binario = argv[1];
	const char *archivo_idf = argv[2];
	const char *archivo_docs = argv[3];
	float k1 = atof(argv[4]);
	float b = atof(argv[5]);
	const char *salida_ubs = argv[6];
	
	InvertedIndex indice;
	indice.load(entrada_indice_binario);
	
	string term;
//	unsigned int term_id;
	InvertedList *lista = NULL;
	BufferedList *buff_list = NULL;
	
	unsigned int buff_size = 256;
	unsigned int *buffer_lista = new unsigned int[buff_size];
	unsigned int *buffer_payloads = new unsigned int[buff_size];
	
	cout<<"Cargando docs...\n";
	map<unsigned int, unsigned int> *mapa_docs = loadDocs(archivo_docs);
	
	cout<<"Cargando idf...\n";
	map<string, float> *mapa_idf = loadIdf(archivo_idf);
	
	cout<<"Preparando datos BM25...\n";
	float bm25_k1 = k1+1;
	map<unsigned int, float> bm25_docs_map;
	map<unsigned int, unsigned int>::iterator it_mapa_docs;
	long double largo_medio = 0;
	unsigned int docid, largo;
	for(it_mapa_docs=mapa_docs->begin(); it_mapa_docs!=mapa_docs->end(); it_mapa_docs++){
		docid = it_mapa_docs->first;
		largo = it_mapa_docs->second;
		largo_medio += largo;
	}
	largo_medio /= mapa_docs->size();
	cout<<"largo_medio: "<<largo_medio<<"\n";
	
	for(it_mapa_docs=mapa_docs->begin(); it_mapa_docs!=mapa_docs->end(); it_mapa_docs++){
		docid = it_mapa_docs->first;
		largo = it_mapa_docs->second;
		//k1*(1-b+b*(largo_doc/largo_medio))
		bm25_docs_map[docid] = k1 * ( 1.0-b + b*((float)largo/largo_medio) );
	}
	
	mapa_docs->clear();
	delete mapa_docs;
	
	cout<<"Preparando UBs...\n";
	map<string, float> mapa_ubs;
	unsigned int payload;
	float ub_term, score = 0;
	unsigned int log2payload;
	
	map<string, InvertedList*> mapa = indice.getMap();
//	map<unsigned int, InvertedList*>::iterator it_mapa;
	
	for( auto it_mapa : mapa ){
		term = it_mapa.first;
		lista = it_mapa.second;
		
		memset(buffer_lista, 0, 512);
		memset(buffer_payloads, 0, 512);
		
		if( (buff_list = dynamic_cast<BufferedList*>(lista)) != NULL ){
			buff_list->setBufferMemory(buffer_lista);
			buff_list = NULL;
		}
		if( (buff_list = dynamic_cast<BufferedList*>(lista->getPayloadList())) != NULL ){
			buff_list->setBufferMemory(buffer_payloads);
			buff_list = NULL;
		}
		
		ub_term = 0.0;
		for(lista->reset(); (docid=lista->getDocId())!=END_OF_STREAM; lista->next()){
			payload = lista->getPayload();
			//calcular score de term/doc
			log2payload = log2bits(payload);
			
			//los parametros de bm25 son necesarios aqui
			
			score = 1.0 * (*mapa_idf)[term] 
				* (float)log2payload * bm25_k1 
				/ ( (float)log2payload + bm25_docs_map[docid]) ;
			
			if(score > ub_term){
				ub_term = score;
			}
			
		}
//		if(term_id%100000 == 0){
//			cout<<"UB["<<term_id<<"]: "<<ub_term<<"\n";
//		}
		mapa_ubs[term] = ub_term;
	}
	
	cout<<"Guardando ubs...\n";
	fstream escritor(salida_ubs, fstream::trunc | fstream::out);
	for( auto it_mapa : mapa){
		term = it_mapa.first;
		lista = it_mapa.second;
		escritor << term << "\t";
		escritor << mapa_ubs[term] << "\n";
	}
	escritor.close();
	
	delete [] buffer_lista;
	delete [] buffer_payloads;
	
	
}













