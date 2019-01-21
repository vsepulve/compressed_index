#if !defined(_WAND_H)
#define _WAND_H

#include <stdio.h>
#include <algorithm>

#include <list>
#include <vector>
#include <set>

#include "ScoringIterator.h"

using namespace std;

class Wand : public ScoringIterator{

protected:
	
	//arreglo para preparar los ubs de cada lista en el init
	double *arr_ubs;
	
	//se pasan como int& a los metodos
	unsigned int currentDocId;
	double currentPayload;
	
	unsigned int *streamDocIds;
	
	// two arrays of indices into the _streams vector. This is used for merge.
	// inplace_merge is not as efficient as the copy merge.
	int *streamIndices;
	int *streamIndicesAux;
	
	// maximum number of streams that WAND can operate on
	int maxNumStreams;
	
	// number of streams present in the query
	int numStreams;
	
	//Lo cambio a arreglo de punteros (no estoy usando nada del vector)
//	vector<InvertedList*> *streams;
	InvertedList **streams;
	
	// scorer
//	Scorer *scorer;
	double sum_ub;
//	vector<QueryFeatureInfo*> *query_terms;
	//solo guardo un puntero, no copio ni destruyo la memoria
	QueryObject *query_terms;
	
	// comparator class that compares two streams. The variables a and b are
	// logically indices into the streams vector.
	class StreamComparator
				: public std::binary_function<int, int, bool> {
	private:
		const unsigned int *streamDocIds;
	public:
		StreamComparator(){
			streamDocIds=NULL;
		}
		StreamComparator(const unsigned int *_streamDocIds){
			streamDocIds=_streamDocIds;
		}
		inline bool operator()(const int a, const int b){
			if (streamDocIds[a] < streamDocIds[b]){
				return true;
			}
			else{
				return false;
			}
		}
	};
	
	// comparator that compares two streams
	// Esto quizas deba ir en una subclase mas especifica
	StreamComparator streamComparator;
	
public:
	
	bool debug;
	
	//Los streams los recibire en el init
	Wand(int _maxNumStreams = 128);
	
	virtual ~Wand();
	
	virtual void init(QueryObject *_query_terms);
	
	virtual unsigned int next(double threshold);
	
	virtual unsigned int getDocId();
	
	virtual double getScore();
	
protected:
	
	//este es redefinido por el score
	virtual void scoreCurrentDoc(int startIdx, int endIdx);
	
	//este es fijo (cuidado con las versiones de puntaje entero)
	//Lo dejo virtual pues puede redefinir como se obtienen los UB
	bool findPivotFeatureIdx(double threshold, int &pivotIdx);
	
	//este es fijo
	void moveStreamsAndSort(const int numStreamsToMove);
	
	//este es fijo
	void sortMerge(const int numStreamsToMove);
	
	//este es fijo
	void moveStreamsToDocAndSort(const int numStreamsToMove, const int desiredDocId);
	
	//este es fijo
	unsigned int log2bits(unsigned int num);
	
};

#endif

