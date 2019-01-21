#include "QueryObject.h"

QueryObject::QueryObject(){
	n_threads = 1;
	id = 0;
	estimated_time = 0.0;
}

QueryObject::~QueryObject(){
	//Las listas son del llamador, por lo que no se destruyen
//	vector<unsigned int> terms;
//	vector<InvertedList*> listas;
//	vector<double> idfs;
//	vector<unsigned int> weights;
	terms.clear();
	listas.clear();
	idfs.clear();
	weights.clear();
}

//De momento, NO ESTOY BORRANDO las listas aqui, deleteLists debe ir ANTES
void QueryObject::clear(){
	terms.clear();
	listas.clear();
	idfs.clear();
	weights.clear();
}

void QueryObject::deleteLists(){
	for(unsigned int i=0; i<listas.size(); ++i){
		delete listas[i];
	}
	//vector<InvertedList*> listas;
}

	
void QueryObject::addData(unsigned int _term_id, InvertedList *_lista, double _idf, unsigned int _weight){
	terms.push_back(_term_id);
	listas.push_back(_lista);
	idfs.push_back(_idf);
	weights.push_back(_weight);
}

unsigned int QueryObject::getTermId(unsigned int pos){
//	if(pos >= terms.size())
//		return 0;
	return terms[pos];
}

InvertedList *QueryObject::getList(unsigned int pos){
//	if(pos >= listas.size())
//		return 0;
	return listas[pos];
}

double QueryObject::getIdf(unsigned int pos){
//	if(pos >= idfs.size())
//		return 0;
	return idfs[pos];
}

unsigned int QueryObject::getWeight(unsigned int pos){
//	if(pos >= weights.size())
//		return 0;
	return weights[pos];
}

		
void QueryObject::setThreads(unsigned int _n_threads){
	n_threads = _n_threads;
}

unsigned int QueryObject::getThreads(){
	return n_threads;
}

		
void QueryObject::setId(unsigned int _id){
	id = _id;
}

unsigned int QueryObject::getId(){
	return id;
}

unsigned int QueryObject::size(){
	return terms.size();
}

void QueryObject::setEstimatedTime(double _estimated_time){
	estimated_time = _estimated_time;
}

double QueryObject::getEstimatedTime(){
	return estimated_time;
}




