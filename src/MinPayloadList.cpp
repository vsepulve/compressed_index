#include "MinPayloadList.h"

MinPayloadList::MinPayloadList(){
	payload = 0;
}

MinPayloadList::MinPayloadList(list<unsigned int> *original_list){
	payload = 0;
	if(original_list != NULL && original_list->size() > 0){
		unsigned int p = *(original_list->begin());
		if(p > 255){
			payload = 255;
		}
		else{
			payload = p;
		}
	}
}

MinPayloadList::MinPayloadList(unsigned int *arr, unsigned int arr_size){
	payload = 0;
	if(arr != NULL && arr_size > 0){
		if(arr[0] > 255){
			payload = 255;
		}
		else{
			payload = arr[0];
		}
	}
}

MinPayloadList::MinPayloadList(MinPayloadList *original){
	payload = original->getPayload(0);
}

MinPayloadList::~MinPayloadList(){
}

void MinPayloadList::reset(){
}

unsigned int MinPayloadList::getPayload(unsigned int pos){
	if(pos == 0){
		return (unsigned int)payload;
	}
	return 0;
}

unsigned int MinPayloadList::size() const{
	return 1;
}

void MinPayloadList::save(fstream *writer){
	writer->write((char*)(&payload), 1);
}

void MinPayloadList::load(fstream *reader){
	reader->read((char*)(&payload), 1);
}

unsigned int MinPayloadList::getSizeData() const{
	//solo 1 byte
	return 1;
}
	
unsigned int MinPayloadList::getSizeBlock() const{
	return 0;
}




