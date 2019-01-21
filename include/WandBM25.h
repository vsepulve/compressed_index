#if !defined(_WAND_BM25_H)
#define _WAND_BM25_H

#include <stdio.h>
#include <algorithm>

#include <list>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>

#include <cmath>

#include "Wand.h"

//PAra debug
#include <fstream>
#include <iostream>

using namespace std;

class WandBM25 : public Wand {

public:
	
//	ofstream *escritor;
	
	//El arreglo de ubs ahora es de Wand	
//	double *arr_ubs;
	map<unsigned int, float> *mapa_ubs;
	
	float bm25_k1;
	//Esto es asumiendo que se pueden acceder por docid (buscare el max_docid)
	float *bm25_docs_map;
//	unordered_map<unsigned int, float> *bm25_docs_map;
//	map<unsigned int, float> bm25_docs_map;
	
	//esto es necesario para que el llamador prepare los UBs
//	unordered_map<unsigned int, float> *getBM25DocsMap(){
	float *getBM25DocsMap(){
		return bm25_docs_map;
	}
	
	float getBM25K1(){
		return bm25_k1;
	}
	
	//Los streams los recibire en el init
	WandBM25(map<unsigned int, unsigned int> *mapa_docs, map<unsigned int, float> *_mapa_ubs = NULL, float k1 = 1.2, float b = 0.75, int _maxNumStreams = 128);
	
//	WandBM25(unordered_map<unsigned int, float> *_bm25_docs_map, map<unsigned int, unsigned int> *mapa_docs, map<unsigned int, float> *_mapa_ubs);
	WandBM25(float *_bm25_docs_map, map<unsigned int, unsigned int> *mapa_docs, map<unsigned int, float> *_mapa_ubs);
	
	~WandBM25();
	
	void init(QueryObject *_query_terms);
	
	//definidos en Wand
	
//	unsigned int next(double threshold);
//	unsigned int getDocId();
//	double getScore();
	
	void setUbsMap(map<unsigned int, float> *_mapa_ubs){
		mapa_ubs = _mapa_ubs;
	}
	
protected : 
	
	void scoreCurrentDoc(int startIdx, int endIdx);
//	bool findPivotFeatureIdx(double threshold, int &pivotIdx);
	
};

#endif

