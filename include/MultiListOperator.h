#ifndef NULTILISTOPERATOR_H
#define NULTILISTOPERATOR_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "InvertedList.h"
#include "QueryObject.h"
#include "ResultObject.h"

using namespace std;

class MultiListOperator{
	private:
	public:
		MultiListOperator();
		virtual ~MultiListOperator();
		virtual void execute(QueryObject *query, vector<ResultObject*> &result);
		
};

#endif //NULTILISTOPERATOR_H
