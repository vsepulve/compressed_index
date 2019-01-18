
#include "MinInvertedList.h"

MinInvertedList::MinInvertedList(){
	doc = 0;
	cur_pos = 0;
}

MinInvertedList::MinInvertedList(list<unsigned int> *original_list){
	doc = 0;
	if(original_list != NULL && original_list->size() > 0){
		doc = *(original_list->begin());
	}
	cur_pos = 0;
}

MinInvertedList::MinInvertedList(unsigned int *arr, unsigned int arr_size){
	doc = 0;
	if(arr != NULL && arr_size > 0){
		doc = arr[0];
	}
	cur_pos = 0;
}

MinInvertedList::MinInvertedList(MinInvertedList *original){
	original->reset();
	doc = original->getDocId();
	cur_pos = 0;
}

MinInvertedList::~MinInvertedList(){
}

void MinInvertedList::save(fstream *escritor){
	escritor->write((char*)(&doc), sizeof(int));
}

void MinInvertedList::load(fstream *lector){
	lector->read((char*)(&doc), sizeof(int));
	reset();
}

void MinInvertedList::reset(){
	cur_pos = 0;
}

unsigned int MinInvertedList::size() const{
	return 1;
}

bool MinInvertedList::hasNext() const{
	return false;
}

unsigned int MinInvertedList::next(){
	cur_pos = 1;
	return END_OF_LIST;
}

unsigned int MinInvertedList::next(unsigned int desired){
	if(desired <= doc){
		return doc;
	}
	else{
		cur_pos = 1;
		return END_OF_LIST;
	}
}

unsigned int MinInvertedList::getDocId() const{
	if( cur_pos > 0){
		return END_OF_LIST;
	}
	return doc;
}

unsigned int MinInvertedList::uncompress(unsigned int *buff){
	buff[0] = doc;
	return 1;
}

//unsigned int *MinInvertedList::getData(){
//	return &doc;
//}

//notar que este metodo NO necesita retornar el entero adicional con el numero de docs
unsigned int MinInvertedList::getSizeData() const{
	return sizeof(int);
}

unsigned int MinInvertedList::getSizeBlock() const{
	return 0;
}






