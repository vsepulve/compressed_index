
#include "PayloadListBuilder.h"

PayloadListBuilder::PayloadListBuilder(){
	buff_size = 1000000;
	buff = new unsigned int[buff_size];
}

PayloadListBuilder::~PayloadListBuilder(){
	if(buff != NULL){
		delete [] buff;
	}
}


//
void PayloadListBuilder::saveList(PayloadList *payload_list, unsigned int max_payload, fstream *writer){
	//if-else con los tipos conocidos
	char type;
	if(payload_list == NULL){
		//sin lista
		type = 0;
		writer->write(&type, 1);
	}
	else if( typeid(*payload_list)==typeid(DummyPayloadList) ){
		//cout<<"PayloadListBuilder::saveList - DummyPayloadList type\n";
		type = 1;
		writer->write(&type, 1);
		writer->write((char*)(&max_payload), sizeof(int));
		payload_list->save(writer);
	}
	else if( typeid(*payload_list)==typeid(PForDeltaPayloadList) ){
		//cout<<"PayloadListBuilder::saveList - PForDeltaPayloadList type\n";
		type = 2;
		writer->write(&type, 1);
		writer->write((char*)(&max_payload), sizeof(int));
		payload_list->save(writer);
	}
//	else if( typeid(*payload_list)==typeid(Simple9PayloadList) ){
//		//cout<<"PayloadListBuilder::saveList - DummyPayloadList type\n";
//		type = 3;
//		writer->write(&type, 1);
//		writer->write((char*)(&max_payload), sizeof(int));
//		payload_list->save(writer);
//	}
	else if( typeid(*payload_list)==typeid(MinPayloadList) ){
		//cout<<"PayloadListBuilder::saveList - DummyPayloadList type\n";
		type = 10;
		writer->write(&type, 1);
		//En este caso, NO escrito el max
		//Al cargar, copio el payload de la lista como el max
//		writer->write((char*)(&max_payload), sizeof(int));
		payload_list->save(writer);
	}
	else{
		cerr<<"PayloadListBuilder::saveList - Unknown list type\n";
		type = 0;
		writer->write(&type, 1);
	}
}

PayloadList *PayloadListBuilder::loadList(fstream *reader, unsigned int &max_payload){
	PayloadList *payload_list = NULL;
	unsigned char type;
	
	reader->read((char*)(&type), 1);
	if(type == 0 || type == 10){
		max_payload = 0;
	}
	else{
		reader->read((char*)(&max_payload), sizeof(int));
	}
	switch(type){
		//en este caso dejo el caso 0 explicitamente como la forma correcta de marcar "no leer lista"
		case 0 : 
//			cout<<"PayloadListBuilder::loadList - No List\n";
			//skip this list
		break;
		case 1 : 
//			cout<<"PayloadListBuilder::loadList - UncompressedList type\n";
			payload_list = new DummyPayloadList();
			payload_list->load(reader);
		break;
		case 2 : 
//			cout<<"PayloadListBuilder::loadList - UncompressedList type\n";
			payload_list = new PForDeltaPayloadList();
			payload_list->load(reader);
		break;
//		case 3 : 
////			cout<<"PayloadListBuilder::loadList - UncompressedList type\n";
//			payload_list=new Simple9PayloadList();
//			payload_list->load(reader);
//		break;
		case 10 : 
//			cout<<"PayloadListBuilder::loadList - UncompressedList type\n";
			payload_list = new MinPayloadList();
			payload_list->load(reader);
			max_payload = payload_list->getPayload(0);
		break;
		default :
			cerr<<"PayloadListBuilder::loadList - Unknown list type ("<<(unsigned int)type<<")\n";
		break;
	}
	return payload_list;
}

PayloadList *PayloadListBuilder::buildList(unsigned char type, list<unsigned int> *original_list){
	if(original_list->size() < 1){
		cerr<<"PayloadListBuilder::buildList - Empty List\n";
		return NULL;
	}
	
	PayloadList *payload_list = NULL;
//	unsigned int *arr = NULL;
	unsigned int pos;
	unsigned int original_size = original_list->size();
	list<unsigned int>::iterator it;
	
	//por ahora dejo los numeros, luego hay que agregar el enum
	switch(type){
		case 1 : 
			//cout<<"PayloadListBuilder::buildList - DummyPayloadList type\n";
			payload_list = new DummyPayloadList(original_list);
		break;
		case 2 : 
			if(original_size > buff_size){
				delete [] buff;
				buff_size = original_size + 1;
				buff = new unsigned int[buff_size];
			}
			pos = 0;
			memset(buff, 0, original_size*sizeof(int));
			for(it=original_list->begin(); it!=original_list->end(); it++){
				buff[pos++] = *it;
			}
			payload_list = new PForDeltaPayloadList(buff, original_size);
		break;
//		case 3 : 
//			//cout<<"PayloadListBuilder::buildList - Simple9PayloadList type\n";
//			arr=new unsigned int[original_list->size()];
//			pos=0;
//			for(it=original_list->begin(); it!=original_list->end(); it++){
//				arr[pos++] = *it;
//			}
//			payload_list=new Simple9PayloadList(arr, original_list->size());
//		break;
		case 10 : 
			//cout<<"PayloadListBuilder::buildList - DummyPayloadList type\n";
			payload_list = new MinPayloadList(original_list);
		break;
		default :
			cerr<<"PayloadListBuilder::buildList - Unknown list type\n";
		break;
	}
//	if(arr!=NULL){
//		delete [] arr;
//	}
	return payload_list;
}


PayloadList *PayloadListBuilder::buildList(unsigned char type, unsigned int *arr, unsigned int arrsize){
	if(arrsize < 1 || arr==NULL){
		cerr<<"PayloadListBuilder::buildList - Empty List\n";
		return NULL;
	}
	
	PayloadList *payload_list=NULL;
	
	//por ahora dejo los numeros, luego hay que agregar el enum
	switch(type){
		case 1 : 
			//cout<<"PayloadListBuilder::buildList - DummyPayloadList type\n";
			payload_list = new DummyPayloadList(arr, arrsize);
		break;
		case 2 : 
			//cout<<"PayloadListBuilder::buildList - PForDeltaPayloadList type\n";
			payload_list = new PForDeltaPayloadList(arr, arrsize);
		break;
//		case 3 : 
//			//cout<<"PayloadListBuilder::buildList - Simple9PayloadList type\n";
//			payload_list=new Simple9PayloadList(arr, arrsize);
//		break;
		case 10 : 
			//cout<<"PayloadListBuilder::buildList - DummyPayloadList type\n";
			payload_list = new MinPayloadList(arr, arrsize);
		break;
		default : 
			cerr<<"PayloadListBuilder::buildList - Unknown list type\n";
		break;
	}
	return payload_list;
}

/*
PayloadList *PayloadListBuilder::getCover(PayloadList *original){
	
	if( typeid(*original)==typeid(PForDeltaPayloadList) ){
		PForDeltaPayloadList *lista_cast = dynamic_cast<PForDeltaPayloadList*>(original);
		PForDeltaPayloadListCover *lista_cover = new PForDeltaPayloadListCover(lista_cast);
		return (PayloadList*)lista_cover;
	}
	else if( typeid(*original)==typeid(MinPayloadList) ){
		MinPayloadList *lista_cast = dynamic_cast<MinPayloadList*>(original);
		MinPayloadList *lista_cover = new MinPayloadList(lista_cast);
		return (PayloadList*)lista_cover;
	}
	else{
		cerr<<"PayloadListBuilder::getCover - Unknown list covering\n";
		return NULL;
	}
}
*/

