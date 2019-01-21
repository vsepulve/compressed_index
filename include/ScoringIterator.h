#if !defined(_SCORING_ITERATOR_H)
#define _SCORING_ITERATOR_H

#include <stdio.h>
#include <algorithm>

#include <list>
#include <vector>
#include <set>

#include "InvertedList.h"
#include "QueryObject.h"

using namespace std;

class ScoringIterator{
public:
	
	ScoringIterator();
	
	virtual ~ScoringIterator();
	
	//prepare the iterator for the current query
	//If it needs more data, it should be passed at construction time
	virtual void init(vector<QueryObject*> *_query_terms);
	
	//advances to the next candidate with a score > threshold
	//it also return the docid of the new candidate
	virtual unsigned int next(double threshold = 0);
	
	//return the docid of the current candidate
	virtual unsigned int getDocId();
	
	//return the score of the current candidate
	virtual double getScore();
	
	
};

#endif

