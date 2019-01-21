
#include "TopKWandOperator.h"

TopKWandOperator::TopKWandOperator(unsigned int _k, unsigned int _max_listas){
//	cout<<"TopKWandOperator - inicio (k: "<<_k<<", max_listas: "<<_max_listas<<")\n";
	k = _k;
	max_listas = _max_listas;
	listas = new InvertedList*[max_listas];
	res.resize(k+1);
	res_indices.resize(k+1);
	comp = new Comparador();
	score_comp = new ScoringComparator(&res);
//	wand = new Wand(max_listas);
	wand = NULL;
	initial_threshold = 0.0;
	
}

void TopKWandOperator::setK(unsigned int _k){
	if(_k > k){
		res.resize(_k+1);
		res_indices.resize(_k+1);
	}
	k = _k;
}

TopKWandOperator::~TopKWandOperator(){
	if(listas!=NULL){
		//El arreglo es solo de punteros
		//No borro las listas pues son del llamador
		delete [] listas;
	}
	delete comp;
	delete wand;
	delete score_comp;
}

void TopKWandOperator::reset(){
	inserts = 0;
	n_res = 0;
	heap_preparado = false;
//	for(unsigned int i=0; i<res.size(); ++i){
//		res[i].setDocId(0);
//		res[i].setScore(0);
//	}
	res[k].setDocId(0);
	res[k].setScore(0);
	
}

//execute que conserva los resultados entre ejecuciones
void TopKWandOperator::execute(QueryObject *query){
	
//cout<<"TopKWandOperator::execute - "<<query->size()<<" terms\n";
	
	//init del wand
	wand->init(query);
	
	//Aqui puede setearse un threshold inicial
	double threshold = initial_threshold;
	
	unsigned int docid = 0;
	double score = 0;
	while ( n_res < k && (docid = wand->next(threshold)) != END_OF_LIST ) {
		score = wand->getScore();
//cout<<"TopKWandOperator::execute - agregando docid: "<<docid<<", score: "<<score<<"\n";
		res_indices[n_res] = n_res;
		
		res[n_res].setDocId(docid);
		res[n_res].setScore(score);
		
		++n_res;
		++inserts;
	}
	
	if(docid == END_OF_LIST){
		//El sort, en cualquier caso, se hace en el getResults
//		std::sort( res_indices.begin(), res_indices.begin() + n_res, *score_comp);
	}
	else{
		// n_res == k, make the heap
		
		if(!heap_preparado){
			res_indices[k] = k;
			std::make_heap(res_indices.begin(), res_indices.begin() + k+1, *score_comp);
			std::pop_heap(res_indices.begin(), res_indices.begin() + k+1, *score_comp);
			heap_preparado = true;
		}
		
		threshold = res[res_indices.front()].getScore();
		
//cout<<"TopKWandOperator::execute - Iterando por el resto de docs (threshold: "<<threshold<<")\n";
		while ( (docid = wand->next(threshold)) != END_OF_LIST) {
			score = wand->getScore();
			
			++inserts;
			
			res[res_indices[k]].setDocId(docid);
			res[res_indices[k]].setScore(score);
			
			std::push_heap(res_indices.begin(), res_indices.begin() + k+1, *score_comp);
			std::pop_heap(res_indices.begin(), res_indices.begin() + k+1, *score_comp);
			
			threshold=res[res_indices.front()].getScore();
//cout<<"TopKWandOperator::execute - nuevo threshold: "<<threshold<<"\n";
			
		}
		
		//En esta version el sort se hace en el getResults
//		std::sort_heap(res_indices.begin(), res_indices.begin() + k, *score_comp);
	}
	
	//Esto tambien se deja en el getResults
//	if(n_res) cout<<"n_res: "<<n_res<<"\n";
//	for(unsigned int i=0; i<n_res; ++i){
//		result[i]->setDocId(res[res_indices[i]].getDocId());
//		result[i]->setScore(res[res_indices[i]].getScore());
//	}
	
	
//	for(unsigned int i=0; i<10; ++i){
//		cout<<"doc["<<i<<"]: "<<res[res_indices[i]].getDocId()<<", "<<res[res_indices[i]].getScore()<<"\n";
//	}
//	cout<<"-----\n";
	
//cout<<"TopKWandOperator::execute - fin\n";
	
}//fin execute

unsigned int TopKWandOperator::getResults(vector<ResultObject*> &result){
	
	if(heap_preparado){
		std::sort_heap( res_indices.begin(), res_indices.begin() + k, *score_comp);
	}
	else{
		res_indices[n_res] = n_res;
		std::sort( res_indices.begin(), res_indices.begin() + n_res, *score_comp);
	}
	
	for(unsigned int i=0; i<n_res; ++i){
		result[i]->setDocId(res[res_indices[i]].getDocId());
		result[i]->setScore(res[res_indices[i]].getScore());
	}
	
	return n_res;
}


void TopKWandOperator::execute(QueryObject *query, vector<ResultObject*> &result){
	
//	cout<<"TopKWandOperator::execute - "<<query->size()<<" terms\n";
	
	//init del wand
	wand->init(query);
	
	inserts = 0;
	n_res = 0;
	res[k].setDocId(0);
	res[k].setScore(0);
	
	//Aqui puede setearse un threshold inicial
	double threshold = initial_threshold;
	
	unsigned int docid = 0;
	double score = 0;
	while ( n_res < k && (docid = wand->next(threshold)) != END_OF_LIST ) {
		score = wand->getScore();
//		cout<<"TopKOperator::execute - docid: "<<docid<<", score: "<<score<<"\n";
		res_indices[n_res] = n_res; 
		
		res[n_res].setDocId(docid);
		res[n_res].setScore(score);
		
		++n_res;
		++inserts;
	}

	if(docid == END_OF_LIST){
		std::sort( res_indices.begin(), res_indices.begin() + n_res, *score_comp);
	}
	else{
		// n_res == k, make the heap
		res_indices[k] = k;
		
		std::make_heap(res_indices.begin(), res_indices.begin() + k+1, *score_comp);
		std::pop_heap(res_indices.begin(), res_indices.begin() + k+1, *score_comp);
		
		threshold = res[res_indices.front()].getScore();
		
//		cout<<"TopKOperator::execute - Iterando por el resto de docs (threshold: "<<threshold<<")\n";
		while ( (docid = wand->next(threshold)) != END_OF_LIST) {
			score = wand->getScore();
			
			++inserts;
			
			res[res_indices[k]].setDocId(docid);
			res[res_indices[k]].setScore(score);
			
			std::push_heap(res_indices.begin(), res_indices.begin() + k+1, *score_comp);
			std::pop_heap(res_indices.begin(), res_indices.begin() + k+1, *score_comp);
			
			threshold=res[res_indices.front()].getScore();
//			cout<<"TopKOperator::execute - threshold: "<<threshold<<"\n";
			
		}

		std::sort_heap(res_indices.begin(), res_indices.begin() + k, *score_comp);
	}
	
//	if(n_res) cout<<"n_res: "<<n_res<<"\n";
	for(unsigned int i=0; i<n_res; ++i){
		result[i]->setDocId(res[res_indices[i]].getDocId());
		result[i]->setScore(res[res_indices[i]].getScore());
	}
	
	
//	for(unsigned int i=0; i<10; ++i){
//		cout<<"doc["<<i<<"]: "<<res[res_indices[i]].getDocId()<<", "<<res[res_indices[i]].getScore()<<"\n";
//	}
//	cout<<"-----\n";
	
//	cout<<"TopKWandOperator::execute - fin\n";
	
}//fin execute












