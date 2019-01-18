
#include <InvertedIndex.h>

InvertedIndex::InvertedIndex(){
}

InvertedIndex::~InvertedIndex(){
	mapa.clear();
}

map<string, InvertedList*> &InvertedIndex::getMap(){
	return mapa;
}

bool InvertedIndex::load(const string &index_file){

	cout << "InvertedIndex::load - Start (from \"" << index_file << "\")\n";
	
	mapa.clear();
	
	// carga de archivo binario
	
	fstream reader;
	unsigned int term_len;
	string term;
	char buff[MAX_WORD];
	unsigned int n_terms;
	InvertedList *lista = NULL;
	PayloadList *lista_payloads = NULL;
//	InvertedListBuilder list_builder;
//	PayloadListBuilder payload_list_builder;
	unsigned int max_payload;
	
//	static int print=0;
	
	reader.open(index_file, fstream::binary | fstream::in);
	reader.read((char*)(&n_terms), sizeof(int));
	cout << "InvertedIndex::load - n_terms: " << n_terms << "\n";
	for(unsigned int i = 0; i < n_terms; ++i){
		reader.read((char*)(&term_len), sizeof(int));
		reader.read(buff, term_len);
		buff[term_len] = 0;
		term = string(buff);
		
		lista = list_builder.loadList(&reader);
		lista_payloads = payload_list_builder.loadList(&reader, max_payload);
		
//		lista->setTermId(term_id);
		
		if(lista_payloads != NULL){
			lista->setPayloadList(lista_payloads, max_payload);
		}
		
		mapa[term] = lista;
	}
	reader.close();
	
//	it_global = mapa->begin();
	
	cout << "InvertedIndex::load - End\n";
	return true;
	
}

/*
bool InvertedIndex::load(const string &index_file, const string &terms_file){

	cout<<"InvertedIndex::load - inicio (desde "<<index_file<<" y terms "<<terms_file<<")\n";
	
	if(mapa!=NULL){
		map<unsigned int, InvertedList*>::iterator it;
		for(it=mapa->begin(); it!=mapa->end(); it++){
			delete it->second;
		}
		mapa->clear();
		delete mapa;
		mapa=NULL;
	}
	mapa = new map<unsigned int, InvertedList*>();
	
	//carga de archivo binario
	
	fstream lector;
	unsigned int term_id, n_terms;
	InvertedList *lista;
	PayloadList *lista_payloads=NULL;
//	InvertedListBuilder list_builder;
//	PayloadListBuilder payload_list_builder;
	unsigned int max_payload;
	
//	static int print=0;
	
	
	set<unsigned int> terms;
	lector.open(terms_file, fstream::in);
//	string linea;
	char *linea = new char[1024];
	
	while(true){
//		getline(entrada, linea);
		lector.getline(linea, 1024);
		if(!lector.good() || strlen(linea)<1){
			break;
		}
		stringstream toks(linea);
		//term_id
		toks>>dec>>term_id;
		terms.insert(term_id);
	}
	lector.close();
	
	lector.open(index_file, fstream::binary | fstream::in);
	lector.read((char*)(&n_terms), sizeof(int));
	cerr<<"InvertedIndex::load - n_terms: "<<n_terms<<" ("<<terms.size()<<" terms cargables)\n";
	fflush(stderr);
	for(unsigned int i=0; i<n_terms; ++i){
//		cout<<"Lista "<<i<<"\n";
		lector.read((char*)(&term_id), sizeof(int));
		lista = list_builder.loadList(&lector);
//		cout<<"term_id: "<<term_id<<", largo: "<<(lista->size())<<"\n";
		
		lista_payloads = payload_list_builder.loadList(&lector, max_payload);
//		if(lista_payloads == NULL){
//			cout<<"lista_payloads nula\n";
//		}
//		else{
//			cout<<"lista_payloads de largo "<<lista_payloads->size()<<"\n";
//		}
		
		lista->setTermId(term_id);
		
		if(lista_payloads != NULL){
			lista->setPayloadList(lista_payloads, max_payload);
		}
		
		if(terms.find(term_id) != terms.end()){
//			cout<<"InvertedIndex::load - Agregando term "<<term_id<<"\n";
			(*mapa)[term_id] = lista;
		}
		else{
			delete lista;
		}
	}
	lector.close();
	
	it_global = mapa->begin();
	
	cerr<<"InvertedIndex::load - fin ("<<mapa->size()<<" terms cargados)\n";
	return true;
	
}
*/

void InvertedIndex::save(const string &index_file){

	cout << "InvertedIndex::save - Start (writing \"" << index_file << "\")\n";
	
	//escribir archivo binario
	
	fstream writer;
	unsigned int term_len;
	string term;
	unsigned int n_terms;
	InvertedList *lista;
//	InvertedListBuilder list_builder;
//	PayloadListBuilder payload_list_builder;
	
	writer.open(index_file, fstream::trunc | fstream::binary | fstream::out);
	
	n_terms = mapa.size();
	writer.write((char*)(&n_terms), sizeof(int));
	
	for( auto it : mapa ){
		term = it.first;
		lista = it.second;
		term_len = term.length();
		writer.write((char*)(&term_len), sizeof(int));
		writer.write((char*)(term.c_str()), term_len);
		list_builder.saveList(lista, &writer);
		payload_list_builder.saveList(lista->getPayloadList(), lista->getMaxPayload(), &writer);
	}
	
	writer.close();
	
	cout << "InvertedIndex::save - End\n";
	
}

InvertedList *InvertedIndex::getList(string &term){
	//cout << "InvertedIndex::getLista - Start (\"" << term << "\")\n";
	
	auto it = mapa.find(term);
	if(it != mapa.end()){
		return it->second;
	}
	else{
		return NULL;
	}
}

/*
InvertedList *InvertedIndex::getListCover(unsigned int term_id){
	//cout<<"InvertedIndex::getLista - inicio ("<<term_id<<")\n";
	
	InvertedList *lista = NULL;
	
	map<unsigned int, InvertedList*>::iterator it;
	it=mapa->find(term_id);
	if(it!=mapa->end()){
		lista = it->second;
	}
	
	if(lista != NULL){
		InvertedList *lista_cover = list_builder.getCover(lista);
		PayloadList *lista_payloads_cover = payload_list_builder.getCover(lista->getPayloadList());
		if(lista_cover != NULL){
			lista_cover->setPayloadList( lista_payloads_cover, lista->getMaxPayload() );
			lista = lista_cover;
		}
	}
	
	return lista;
}
*/

void InvertedIndex::addList(string &term, InvertedList *inverted_list){
	auto it = mapa.find(term);
	if(it != mapa.end()){
		delete it->second;
		mapa.erase(it);
	}
//	inverted_list->setTermId(term_id);
	mapa[term] = inverted_list;
}

unsigned int InvertedIndex::size(){
	return mapa.size();
}

//Metodos para iterar por las listas del indice
/*
void InvertedIndex::reset(){
	it_global = mapa->begin();
}

InvertedList *InvertedIndex::getCurList(){
	if(it_global != mapa->end()){
		return it_global->second;
	}
	else{
		return NULL;
	}
}

unsigned int InvertedIndex::getCurTerm(){
	if(it_global != mapa->end()){
		return it_global->first;
	}
	else{
		return 0;
	}
}

InvertedList *InvertedIndex::next(){
	if(it_global != mapa->end()){
		it_global++;
		if(it_global != mapa->end()){
			return it_global->second;
		}
	}
	return NULL;
}
*/











