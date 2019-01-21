#include "ScoringIterator.h"

ScoringIterator::ScoringIterator(){}

ScoringIterator::~ScoringIterator(){}

void ScoringIterator::init(vector<QueryObject*> *_query_terms){}

unsigned int ScoringIterator::next(double threshold){
	return 0;
}

unsigned int ScoringIterator::getDocId(){
	return 0;
}

double ScoringIterator::getScore(){
	return 0.0;
}
