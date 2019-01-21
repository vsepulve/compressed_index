#ifndef _TOPK_WAND_OPERATOR_H
#define _TOPK_WAND_OPERATOR_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#include "MultiListOperator.h"
#include "Wand.h"

using namespace std;

class TopKWandOperator : public MultiListOperator{
	private:
		//arreglo de punteros para facilitar el acceso a las listas durante la consulta
		unsigned int max_listas;
		InvertedList **listas;
		unsigned int k;
		vector<ResultObject> res;
		vector<unsigned int> res_indices;
		unsigned int n_res;
		bool heap_preparado;
		
	class Comparador : public std::binary_function<const InvertedList*, const InvertedList*, bool> {
	public:
		Comparador(){
		}
		inline bool operator()(const InvertedList *a, const InvertedList *b){
			if (a->size() < b->size()){
				return true;
			}
			else if (a->size() > b->size()){
				return false;
			}
			else{
				return (a->getTermId() < b->getTermId());
			}
		}
	};
	
	class ScoringComparator : public std::binary_function<unsigned int, unsigned int, bool> {
	public:
		ScoringComparator(const vector<ResultObject> *_res){
			res=_res;
		}
		inline bool operator()(const unsigned int a, const unsigned int b){
			if((*res)[a].getScore() == (*res)[b].getScore()){
				return (*res)[a].getDocId() > (*res)[b].getDocId();
			}
			return (*res)[a].getScore() > (*res)[b].getScore();
		}
	private:
		const std::vector<ResultObject> *res;
	};
	
	ScoringComparator *score_comp;
	Comparador *comp;
	
	Wand *wand;
		
	public:
		
		unsigned int inserts;
		double initial_threshold;
		
		TopKWandOperator(unsigned int _k = 10, unsigned int _max_listas = 128);
		~TopKWandOperator();
		virtual void execute(QueryObject *query, vector<ResultObject*> &result);
		
		//resetea las variables locales como al inicio de un execute normal
		void reset();
		//execute que conserva los resultados (no los retorna)
		void execute(QueryObject *query);
		//peticion de resultados como al final de un execute normal
		unsigned int getResults(vector<ResultObject*> &result);
		
		void setWand(Wand *_wand){ wand = _wand;}
		Wand *getWand(){ return wand;}
		void setInitialThreshold(double _initial_threshold){ initial_threshold = _initial_threshold;}
		unsigned int getInserts(){ return inserts;}
		
		void setK(unsigned int _k);
		
		
};

#endif //SIMPLEANDOPERATOR_H
