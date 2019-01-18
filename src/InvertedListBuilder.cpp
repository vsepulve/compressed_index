
#include "InvertedListBuilder.h"

InvertedListBuilder::InvertedListBuilder(){
	buff_size = 1000000;
	buff = new unsigned int[buff_size];
}

InvertedListBuilder::~InvertedListBuilder(){
	if(buff != NULL){
		delete [] buff;
	}
}

// Tipo 1 es unc
// Los tipos 2-4 quedaran reservados para VB-S9-P4D.
// Los tipos 5-7 son las tecnicas en el mismo orden con RL.
// Tipo 8 es interpolative
// Tipo 10 es MIN
void InvertedListBuilder::saveList(InvertedList *inverted_list, fstream *writer){
	//if-else con los tipos conocidos
	char type;
	if( typeid(*inverted_list)==typeid(UncompressedList) ){
		//cout<<"InvertedListBuilder::saveList - UncompressedList type\n";
		type = 1;
		writer->write(&type, 1);
		inverted_list->save(writer);
	}
	else if( typeid(*inverted_list)==typeid(VarByteList) ){
		type = 2;
		writer->write(&type, 1);
		inverted_list->save(writer);
	}
	else if( typeid(*inverted_list)==typeid(Simple9) ){
		type = 3;
		writer->write(&type, 1);
		inverted_list->save(writer);
	}
	else if( typeid(*inverted_list)==typeid(PForDelta) ){
		type = 4;
		writer->write(&type, 1);
		inverted_list->save(writer);
	}
//	else if( typeid(*inverted_list)==typeid(RLVarByteList) ){
//		type=5;
//		writer->write(&type, 1);
//		inverted_list->save(writer);
//	}
//	else if( typeid(*inverted_list)==typeid(rl_s9) ){
//		type=6;
//		writer->write(&type, 1);
//		inverted_list->save(writer);
//	}
//	else if( typeid(*inverted_list)==typeid(RLPForDelta) ){
//		type=7;
//		writer->write(&type, 1);
//		inverted_list->save(writer);
//	}
//	else if( typeid(*inverted_list)==typeid(interpolative) ){
//		type=8;
//		writer->write(&type, 1);
//		inverted_list->save(writer);
//	}
	else if( typeid(*inverted_list)==typeid(MinInvertedList) ){
		type = 10;
		writer->write(&type, 1);
		inverted_list->save(writer);
	}
	else{
		cout<<"InvertedListBuilder::saveList - Unknown list type\n";
		type=0;
		writer->write(&type, 1);
	}
}

InvertedList *InvertedListBuilder::loadList(fstream *reader){
	InvertedList *inverted_list=NULL;
	unsigned char type;
	
	reader->read((char*)(&type), 1);
	switch(type){
		case 1 : 
//cout<<"InvertedListBuilder::loadList - UncompressedList type\n";
			inverted_list=new UncompressedList();
			inverted_list->load(reader);
		break;
		case 2:
//cout<<"InvertedListBuilder::loadList - VarByteList type\n";
			inverted_list=new VarByteList();
			inverted_list->load(reader);
		break;
		case 3 : 
////cout<<"InvertedListBuilder::loadList - s9 type\n";
			inverted_list=new Simple9();
			inverted_list->load(reader);
		break;
		case 4 : 
//cout<<"InvertedListBuilder::loadList - PForDelta type\n";
			inverted_list=new PForDelta();
			inverted_list->load(reader);
		break;
//		case 5:
////cout<<"InvertedListBuilder::loadList - RLVarByteList type\n";
//			inverted_list=new RLVarByteList();
//			inverted_list->load(reader);
//		break;
//		case 6 : 
////cout<<"InvertedListBuilder::loadList - rls9 type\n";
//			inverted_list=new rl_s9();
//			inverted_list->load(reader);
//		break;
//		case 7 : 
////cout<<"InvertedListBuilder::loadList - RLPForDelta type\n";
//			inverted_list=new RLPForDelta();
//			inverted_list->load(reader);
//		break;
//		case 8 : 
////cout<<"InvertedListBuilder::loadList - interpolative type\n";
//			inverted_list=new interpolative();
//			inverted_list->load(reader);
//		break;
		case 10:
//cout<<"InvertedListBuilder::loadList - VarByteList type\n";
			inverted_list = new MinInvertedList();
			inverted_list->load(reader);
		break;
		default :
			cerr<<"InvertedListBuilder::loadList - Unknown list type ("<<(unsigned int)type<<")\n";
		break;
	}
	return inverted_list;
}

InvertedList *InvertedListBuilder::buildList(unsigned char type, list<unsigned int> *original_list){
	if(original_list->size() < 1){
		cout<<"InvertedListBuilder::buildList - Empty List\n";
		return NULL;
	}
	
	InvertedList *inverted_list = NULL;
//	unsigned int *arr = NULL;
	unsigned int pos;
	unsigned int original_size = original_list->size();
	list<unsigned int>::iterator it;
	
	//por ahora dejo los numeros, luego hay que agregar el enum
	switch(type){
		case 1 : 
			//cout<<"InvertedListBuilder::buildList - UncompressedList type\n";
			inverted_list = new UncompressedList(original_list);
		break;
		case 2 : 
			inverted_list = new VarByteList(original_list);
		break;
		case 3 : 
			if(original_size > buff_size){
				delete [] buff;
				buff_size = original_size + 1;
				buff = new unsigned int[buff_size];
			}
			pos = 0;
			memset(buff, 0, original_size*sizeof(int));
			for(it = original_list->begin(); it != original_list->end(); it++){
				buff[pos++] = (*it);
			}
			inverted_list = new Simple9(buff, original_size);
		break;
		case 4 :
			if(original_size > buff_size){
				delete [] buff;
				buff_size = original_size + 1;
				buff = new unsigned int[buff_size];
			}
			pos = 0;
			memset(buff, 0, original_size*sizeof(int));
			for(it = original_list->begin(); it != original_list->end(); it++){
				buff[pos++] = (*it);
			}
			inverted_list = new PForDelta(buff, original_size);
		break;
//		case 5 : 
//			inverted_list=new RLVarByteList(original_list);
//		break;
//		case 6 : 
//			arr=new unsigned int[original_size];
//			pos=0;
//			for(it=original_list->begin(); it!=original_list->end(); it++){
//				arr[pos++]=(*it);
//			}
//			inverted_list=new rl_s9(arr, original_size);
//		break;
//		case 7 : 
//			arr=new unsigned int[original_size];
//			pos=0;
//			for(it=original_list->begin(); it!=original_list->end(); it++){
//				arr[pos++]=(*it);
//			}
//			inverted_list=new RLPForDelta(arr, original_size);
//		break;
//		case 8 : 
//			arr=new unsigned int[original_size];
//			pos=0;
//			for(it=original_list->begin(); it!=original_list->end(); it++){
//				arr[pos++]=(*it);
//			}
//			inverted_list=new interpolative(arr, original_size);
//		break;
		case 10 : 
			inverted_list = new MinInvertedList(original_list);
		break;
		default :
			cout<<"InvertedListBuilder::buildList - Unknown list type\n";
		break;
	}
	return inverted_list;
}


InvertedList *InvertedListBuilder::buildList(unsigned char type, unsigned int * arr, unsigned int arrsize){
	if(arrsize < 1){
		cout<<"InvertedListBuilder::buildList - Empty List\n";
		return NULL;
	}
	
	InvertedList *inverted_list=NULL;
	
	//por ahora dejo los numeros, luego hay que agregar el enum
	switch(type){
		case 1 : 
			//cout<<"InvertedListBuilder::buildList - UncompressedList type\n";
			inverted_list = new UncompressedList(arr, arrsize);
		break;
		case 2 : 
			inverted_list = new VarByteList(arr, arrsize);
		break; 
		case 3 : 
			inverted_list = new Simple9(arr, arrsize);
		break; 
		case 4 : 
			inverted_list = new PForDelta(arr, arrsize);
		break; 
//		case 5 : 
//			inverted_list = new RLVarByteList(arr, arrsize);
//		break; 
//		case 6 : 
//			inverted_list=new rl_s9(arr, arrsize);
//		break; 
//		case 7 : 
//			inverted_list=new RLPForDelta(arr, arrsize);
//		break;
//		case 8 : 
//			inverted_list=new interpolative(arr, arrsize);
//		break;
		case 10 : 
			inverted_list = new MinInvertedList(arr, arrsize);
		break; 
		default :
			cout<<"InvertedListBuilder::buildList - Unknown list type\n";
		break;
	}
	return inverted_list;
}

/*
InvertedList *InvertedListBuilder::getCover(InvertedList *original){
	
	if( typeid(*original)==typeid(UncompressedList) ){
		UncompressedList *lista_cast = dynamic_cast<UncompressedList*>(original);
		UncompressedListCover *lista_cover = new UncompressedListCover(lista_cast);
		return (InvertedList*)lista_cover;
	}
	else if( typeid(*original)==typeid(VarByteList) ){
		VarByteList *lista_cast = dynamic_cast<VarByteList*>(original);
		VarByteListCover *lista_cover = new VarByteListCover(lista_cast);
		return (InvertedList*)lista_cover;
	}
	else if( typeid(*original)==typeid(MinInvertedList) ){
		MinInvertedList *lista_cast = dynamic_cast<MinInvertedList*>(original);
		MinInvertedList *lista_cover = new MinInvertedList(lista_cast);
		return (InvertedList*)lista_cover;
	}
	else{
		cerr<<"InvertedListBuilder::getCover - Unknown list covering\n";
		return NULL;
	}
}
*/

