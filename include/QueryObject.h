#ifndef QUERYOBJECT_H
#define QUERYOBJECT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <vector>

#include "InvertedList.h"

using namespace std;

class QueryObject{
	protected:
		vector<unsigned int> terms;
		vector<InvertedList*> listas;
		vector<unsigned int> weights;
		vector<double> idfs;
		
		unsigned int n_threads;
		unsigned int id;
		
		double estimated_time;
		
	public:
		QueryObject();
		virtual ~QueryObject();
		
		void clear();
		
		void addData(unsigned int _term_id, InvertedList *_lista = NULL, double _idf = 0.0, unsigned int _weight = 1);
		
		unsigned int getTermId(unsigned int pos);
		
		InvertedList *getList(unsigned int pos);
		
		double getIdf(unsigned int pos);
		
		unsigned int getWeight(unsigned int pos);
		
		void setThreads(unsigned int _n_threads);
		
		unsigned int getThreads();
		
		void setId(unsigned int _id);
		
		unsigned int getId();
		
		unsigned int size();
		
		void setEstimatedTime(double _estimated_time);
		
		double getEstimatedTime();
		
		//Borrar efectivamente las listas
		//Esto es mientras pruebo el sistema de ListCover
		//En el largo plazo, deberia ser parte del destructor
		void deleteLists();
		
//		inline void setList(unsigned int pos, InvertedList *_lista = NULL){
//			listas[pos] = _lista;
//		}
		
		//Esto era para el TopkMultithread, lo reviso despues
//		inline void setValues(QueryObject *o){
//			term_id = o->term_id;
//			lista = o->lista;
//			weight = o->weight;
////			ub = o->ub;
//			idf = o->idf;
//		}
		
};

#endif //QUERYOBJECT_H
