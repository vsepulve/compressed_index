#include "DummyPayloadList.h"

DummyPayloadList::DummyPayloadList(){
	n_payloads=0;
	payloads=NULL;
}

DummyPayloadList::DummyPayloadList(list<unsigned int> *original_list){
//	cout<<"DummyPayloadList\n";
	n_payloads=original_list->size();
	payloads=new unsigned char[n_payloads];
	list<unsigned int>::iterator it;
	unsigned int pos=0;
	for(it=original_list->begin(); it!=original_list->end(); it++){
		if(*it > 255){
			payloads[pos++]=255;
		}
		else{
			payloads[pos++]=(unsigned char)(*it);
		}
	}
}

DummyPayloadList::DummyPayloadList(unsigned int *arr, unsigned int arr_size){
//	cout<<"DummyPayloadList\n";
	n_payloads=arr_size;
	payloads=new unsigned char[n_payloads];
	for(unsigned int i=0; i<n_payloads; ++i){
		if(arr[i] > 255){
			payloads[i]=255;
		}
		else{
			payloads[i]=(unsigned char)(arr[i]);
		}
	}
}

DummyPayloadList::~DummyPayloadList(){
	n_payloads=0;
	if(payloads!=NULL){
		delete [] payloads;
	}
}

void DummyPayloadList::reset(){
}

unsigned int DummyPayloadList::getPayload(unsigned int pos){
	//para fines experimentales, podemos asumir correctitud y comentar el if de seguridad
	//probablemente eso sera replicable en la tecnica real
//	if(pos >= n_payloads){
//		return 0;
//	}
	return (unsigned int)(payloads[pos]);
}

unsigned int DummyPayloadList::size() const{
	return n_payloads;
}

void DummyPayloadList::save(fstream *writer){
	writer->write((char*)(&n_payloads), sizeof(int));
	if(n_payloads>0){
		writer->write((char*)(payloads), n_payloads);
	}
}

void DummyPayloadList::load(fstream *reader){
	n_payloads=0;
	if(payloads!=NULL){
		delete [] payloads;
		payloads=NULL;
	}
	reader->read((char*)(&n_payloads), sizeof(int));
	if(n_payloads>0){
		payloads=new unsigned char[n_payloads];
		reader->read((char*)(payloads), n_payloads);
	}
	reset();
}

unsigned int DummyPayloadList::getSizeData() const{
	return sizeof(int) + n_payloads;
}

unsigned int DummyPayloadList::getSizeBlock() const{
	return 0;
}



