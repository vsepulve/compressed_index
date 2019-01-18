
#include "InvertedIndexBuilder.h"

InvertedIndexBuilder::InvertedIndexBuilder(){
}

InvertedIndexBuilder::~InvertedIndexBuilder(){
}

InvertedIndex *InvertedIndexBuilder::createIndex(const string &text_file, unsigned char list_type, unsigned char payload_type, map<unsigned int, unsigned int> *renumeration){

	cout << "InvertedIndexBuilder::createIndex - Start (from \"" << text_file << "\", list_type: " << list_type << ", payload_type: " << payload_type << ")\n";
	
	InvertedListBuilder list_builder;
	PayloadListBuilder payload_list_builder;
	
	InvertedIndex *index = new InvertedIndex();
	InvertedList *inverted_list = NULL;
	PayloadList *payload_list = NULL;
	
	ifstream entrada;
	entrada.open(text_file);
	string linea, palabra;
	string term;
	unsigned int n_docs;
	unsigned int docid;
	vector< pair<unsigned int, unsigned int> > read_list;
	list<unsigned int> unc_list;
	list<unsigned int> unc_payload_list;
	
	unsigned int payload;
	
	while(true){
		getline(entrada, linea);
		if(!entrada.good() || linea.size()<1){
			break;
		}
		stringstream toks(linea);
		
		// term
		toks >> term;
		
		// n_docs
		toks >> n_docs;
		
//		cout << "InvertedIndexBuilder::createIndex - n_docs: " << n_docs << "\n";
		for(unsigned int i = 0; i < n_docs; ++i){
			toks >> docid;
			toks >> payload;
			if( renumeration != NULL ){
				auto it = renumeration->find(docid);
				if( it != renumeration->end() ){
					docid = it->second;
				}
				else{
					cout << "InvertedIndexBuilder::createIndex - Omiting docid " << docid << " (not found in renumeration)\n";
					docid = (unsigned int)(-1);
				}
			}
			if( docid != (unsigned int)(-1) ){
				read_list.push_back( pair<unsigned int, unsigned int>(docid, payload) );
			}
		}
		
		std::sort(read_list.begin(), read_list.end(), 
			[](const pair<unsigned int, unsigned int> &p1, const pair<unsigned int, unsigned int> &p2) 
				-> bool{return p1.first < p2.first;}
		);
		
		unsigned int last_docid = 0;
		unsigned int max_payload = 0;
		unsigned int delta = 0;
		for( auto par : read_list ){
			docid = par.first;
			payload = par.second;
			delta = docid - last_docid;
			last_docid = docid;
			if( payload > max_payload ){
				max_payload = payload;
			}
			unc_list.push_back(delta);
			unc_payload_list.push_back(payload);
		}
		
		inverted_list = list_builder.buildList(list_type, &unc_list);
		payload_list = payload_list_builder.buildList(payload_type, &unc_payload_list);
		
		unc_list.clear();
		unc_payload_list.clear();
		
		if( inverted_list != NULL ){
			if( payload_list != NULL ){
				inverted_list->setPayloadList(payload_list, max_payload);
			}
			index->addList(term, inverted_list);
		}
		else{
			cout << "InvertedIndexBuilder::createIndex - Omiting list for term \"" << term << "\" (NULL list)\n";
		}
		
	}
	entrada.close();
	
	cout << "InvertedIndexBuilder::createIndex - End (n_terms: " << index->size() << ")\n";
	return index;
}

InvertedIndex *InvertedIndexBuilder::createIndex(const string &text_file, unsigned char list_type){
	return createIndex(text_file, list_type, 2, NULL);
}

InvertedIndex *InvertedIndexBuilder::createIndex(const string &text_file, unsigned char list_type, unsigned char payload_type){
	return createIndex(text_file, list_type, payload_type, NULL);
}
	
InvertedIndex *InvertedIndexBuilder::createIndex(const string &text_file, unsigned char list_type, const string &renum_file){
	map<unsigned int, unsigned int> renumeration;
	loadRenumerationTable(renum_file, &renumeration);
	return createIndex(text_file, list_type, 2, &renumeration);
}

InvertedIndex *InvertedIndexBuilder::createIndex(const string &text_file, unsigned char list_type, unsigned char payload_type, const string &renum_file){
	map<unsigned int, unsigned int> renumeration;
	loadRenumerationTable(renum_file, &renumeration);
	return createIndex(text_file, list_type, payload_type, &renumeration);
}

void InvertedIndexBuilder::loadRenumerationTable(const string &renum_file, map<unsigned int, unsigned int> *renumeration){
	ifstream reader;
	reader.open(renum_file);
	unsigned int old_docid, new_docid;
	unsigned int buff_size = 1024;
	char buff[buff_size];
	while( reader.good() ){
		reader.getline(buff, buff_size);
		unsigned int n_read = reader.gcount();
		if( n_read <= 1 ){
			continue;
		}
		string line(buff);
		stringstream toks(line);
		toks >> old_docid;
		toks >> new_docid;
		(*renumeration)[old_docid] = new_docid;
	}
	reader.close();
}













