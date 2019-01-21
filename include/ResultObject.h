#ifndef RESULTOBJECT_H
#define RESULTOBJECT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class ResultObject{
	protected:
		unsigned int docid;
		double valor;
	public:
		ResultObject();
		ResultObject(unsigned int _docid, double _valor);
		~ResultObject();
		
		inline unsigned int getDocId() const {return docid;}
		inline double getScore() const {return valor;}
		
		inline void setDocId(unsigned int _docid){docid=_docid;}
		inline void setScore(double _valor){valor=_valor;}
		
};

#endif //RESULTOBJECT_H
