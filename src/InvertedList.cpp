
#include "InvertedList.h"

InvertedList::InvertedList(){
	payload_list = NULL;
	max_payload = 0;
}

InvertedList::InvertedList(list<unsigned int> *original_list){
	payload_list = NULL;
	max_payload = 0;
}

InvertedList::InvertedList(unsigned int *arr, unsigned int arr_size){
	payload_list = NULL;
	max_payload = 0;
}

InvertedList::~InvertedList(){
	if(payload_list != NULL){
		delete payload_list;
		payload_list = NULL;
	}
}

void InvertedList::save(fstream *escritor){
}

void InvertedList::load(fstream *lector){
}

void InvertedList::reset(){
}

unsigned int InvertedList::size() const{
	return 0;
}

bool InvertedList::hasNext(){
	return false;
}

unsigned int InvertedList::next(){
	return 0;
}

unsigned int InvertedList::next(unsigned int desired){
	return 0;
}

unsigned int InvertedList::getDocId() const{
	return 0;
}

unsigned int InvertedList::uncompress(unsigned int *buff){
	return 0;
}

void InvertedList::setTermId(unsigned int _term_id){
	term_id=_term_id;
}

unsigned int InvertedList::getTermId() const{
	return term_id;
}

unsigned int InvertedList::getSizeData() const{
	return 0;
}

unsigned int InvertedList::getSizeBlock() const{
	return 0;
}

unsigned int InvertedList::getPayload() const{
	//el if siguiente puede sacarse para fines experimentales si se sabe que habra frecuencias
//	if(payload_list==NULL){
//		return 0;
//	}
	
//	cout<<"InvertedList::getPayload - cur_pos: "<<cur_pos<<"\n";
	
	return payload_list->getPayload(cur_pos);
	
}

unsigned int InvertedList::getMaxPayload() const{
	return max_payload;
}

void InvertedList::setPayloadList(PayloadList *_payload_list, unsigned int _max_payload){
	if(_payload_list == NULL){
//		cout<<"InvertedList::setPayloadList - Lista NULL\n";
		return;
	}
	if(_payload_list->size() != this->size()){
		cerr<<"InvertedList::setPayloadList - listas de diferente largo ("<<_payload_list->size()<<" de "<<size()<<")\n";
		return;
	}
	payload_list = _payload_list;
	max_payload = _max_payload;
//	cout<<"InvertedList::setPayloadList - Lista de largo "<<(payload_list->size())<<"\n";
	
//	payload_list->reset();
//	max_payload=0;
//	unsigned int payload;
//	for(unsigned int i=0; i<size(); ++i){
//		payload=payload_list->getPayload(i);
//		if(payload > max_payload){
//			max_payload=payload;
//		}
//	}
//	payload_list->reset();
}


PayloadList * InvertedList::getPayloadList(){
	return payload_list;
}





