
#include "UncompressedList.h"

UncompressedList::UncompressedList(){
	n_docs = 0;
	docs = NULL;
	cur_pos = 0;
}

UncompressedList::UncompressedList(list<unsigned int> *original_list){
	n_docs=original_list->size();
	docs=new unsigned int[n_docs];
	list<unsigned int>::iterator it;
	
	it=original_list->begin();
	docs[0]=*it;
	it++;
	cur_pos=1;
	for(; it!=original_list->end(); it++){
		docs[cur_pos]=(*it)+docs[cur_pos-1];
		cur_pos++;
	}
	
	cur_pos=0;
}

UncompressedList::UncompressedList(unsigned int *arr, unsigned int arr_size){
	n_docs=arr_size;
	docs=new unsigned int[n_docs];
	
	memcpy(docs, arr, n_docs*sizeof(int));
	for(unsigned int i=1; i<n_docs; ++i){
		docs[i]+=docs[i-1];
	}
	
	cur_pos=0;
}

UncompressedList::~UncompressedList(){
	if(docs != NULL){
		delete [] docs;
		docs = NULL;
	}
}

void UncompressedList::save(fstream *escritor){
	escritor->write((char*)(&n_docs), sizeof(int));
	if(n_docs>0){
		escritor->write((char*)(docs), n_docs*sizeof(int));
	}
}

void UncompressedList::load(fstream *lector){
	if(docs!=NULL){
		delete [] docs;
	}
	lector->read((char*)(&n_docs), sizeof(int));
	if(n_docs>0){
		docs=new unsigned int[n_docs];
		lector->read((char*)(docs), n_docs*sizeof(int));
	}
	reset();
}

void UncompressedList::reset(){
	cur_pos=0;
	if(payload_list != NULL){
		payload_list->reset();
	}
}

unsigned int UncompressedList::size() const{
	return n_docs;
}

bool UncompressedList::hasNext() const{
	return cur_pos<(n_docs-1);
}

unsigned int UncompressedList::next(){
	if(cur_pos < n_docs-1){
		cur_pos++;
		return docs[cur_pos];
	}
	else{
		cur_pos=n_docs;
		return END_OF_LIST;
	}
}

unsigned int UncompressedList::next(unsigned int desired){
	unsigned int cur_doc=getDocId();
//	cout<<"UncompressedList::next - inicio (term_id: "<<term_id<<", cur_doc: "<<cur_doc<<")\n";
	if(desired<=cur_doc){
//		cout<<"UncompressedList::next("<<desired<<" en term "<<term_id<<"): "<<cur_doc<<" [cur_doc]\n";
		return cur_doc;
	}
	if(desired>docs[n_docs-1]){
		cur_pos=n_docs;
//		cout<<"UncompressedList::next("<<desired<<" en term "<<term_id<<"): "<<END_OF_LIST<<" [EOL]\n";
		return END_OF_LIST;
	}
	
	
	//Prueba secuencial
	while(docs[++cur_pos]<desired) ;


//	//cur_doc < desired <= last
//	unsigned int l=cur_pos;
//	unsigned int h=n_docs-1;
//	unsigned int m;
//	while(l<h){
////		m=l+((h-l)/2);
//		m = l + ((h-l)>>1);
//		if(docs[m]<desired){
//			l=m+1;
//		}
//		else{
//			h=m;
//		}
//	}
//	//h == l
//	cur_pos=h;

	
//	cout<<"UncompressedList::next("<<desired<<" en term "<<term_id<<"): "<<docs[cur_pos]<<" [normal]\n";
	return docs[cur_pos];
}

unsigned int UncompressedList::getDocId() const{
	if(cur_pos==n_docs){
		return END_OF_LIST;
	}
	return docs[cur_pos];
}

unsigned int UncompressedList::uncompress(unsigned int *buff){
	memcpy(buff, docs, n_docs*sizeof(int));
	return n_docs;
}

unsigned int *UncompressedList::getData(){
	return docs;
}

//notar que este metodo NO necesita retornar el entero adicional con el numero de docs
unsigned int UncompressedList::getSizeData() const{
	return n_docs*sizeof(int);
}

unsigned int UncompressedList::getSizeBlock() const{
	return 0;
}






