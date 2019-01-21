#if !defined(_WAND_TFIDF_H)
#define _WAND_TFIDF_H

#include <stdio.h>
#include <algorithm>

#include <list>
#include <vector>
#include <set>

#include <cmath>

#include "Wand.h"

using namespace std;

class WandTfidf : public Wand {
public:
	
	//se pasan como int& a los metodos
//	unsigned int currentDocId;
//	float currentPayload;
	
//	unsigned int *streamDocIds;
	
//	// two arrays of indices into the _streams vector. This is used for merge.
//	// inplace_merge is not as efficient as the copy merge.
//	int *streamIndices;
//	int *streamIndicesAux;
	
//	// maximum number of streams that WAND can operate on
//	int maxNumStreams;
	
//	// number of streams present in the query
//	int numStreams;
	
//	//Lo cambio a arreglo de punteros (no estoy usando nada del vector)
////	vector<InvertedList*> *streams;
//	InvertedList **streams;
	
//	float sum_ub;
//	vector<QueryFeatureInfo*> *query_terms;
	//solo guardo un puntero, no copio ni destruyo la memoria
//	vector<QueryObject*> *query_terms;
	
	//Los streams los recibire en el init
	WandTfidf(int _maxNumStreams = 128);
	
	~WandTfidf();
	
	void init(QueryObject *_query_terms);
	
//	unsigned int next(double threshold);
	
	//definido en Wand
//	unsigned int getDocId();
	
//	double getScore();
	
protected :
	
//	bool findPivotFeatureIdx(const float threshold, int &pivotIdx);
//	
//	void moveStreamsAndSort(const int numStreamsToMove);
//	
//	void sortMerge(const int numStreamsToMove);

	void scoreCurrentDoc(int startIdx, int endIdx);
	
	
};

#endif

