#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>

#include <algorithm>

#include <map>
#include <vector>
#include <list>

using namespace std;

int main(int argc, char* argv[]){

	if(argc != 3){
		cout << "\n";
		cout << "Usage: index_simple input_text output_index\n";
		cout << "This program indexes a text file considering each line as a \"document\" and writes it in text format\n";
		cout << "The whole index is loaded in memory while constructing\n";
		cout << "Line number (starting with 1) are used as docids\n";
		cout << "\n";
		return 0;
	}
	
	const char *input = argv[1];
	const char *output = argv[2];
	
	cout << "Start\n";
	
	// Indice basico: term -> list <docid, frec>
	map<string, list<pair<unsigned int, unsigned int>>> index;
	
	// Max "document" size of 100MB for simple and fast reading
	unsigned int buff_size = 100*1024*1024;
	char *buff = new char[buff_size + 1];
	
	string word = "";
	unsigned int docid = 0;
	unsigned int frec = 0;
	
	cout << "Reading \"" << input << "\"\n";
	
	fstream reader(input, fstream::in);
	
	unsigned int next_doc = 0;
	while( reader.good() ){
		
		// Linea 
		docid = ++next_doc;
		
		reader.getline(buff, buff_size);
		unsigned int n_read = reader.gcount();
		if( n_read <= 1 ){
			continue;
		}
		
		string line(buff);
		
		// Potencialmente agregar otros separadores
		replace(line.begin(), line.end(), '.', ' ');
		replace(line.begin(), line.end(), ',', ' ');
		replace(line.begin(), line.end(), ';', ' ');
		replace(line.begin(), line.end(), '"', ' ');
		replace(line.begin(), line.end(), '\'', ' ');
		replace(line.begin(), line.end(), '(', ' ');
		replace(line.begin(), line.end(), ')', ' ');
		
		// Notar que acentos, ñ y otros caracteres especiales 
		//   NO esta asegurado que funcionen con este metodo
		for(unsigned int i = 0; i < line.length(); ++i){
			line[i] = toupper(line[i]);
		}
		
		stringstream toks(line);
		
		// Conteo de palabras en mapa del documento
		map<string, unsigned int> doc_map;
		while( toks.good() ){
			toks >> word;
			if( word.length() > 0 ){
				doc_map[word]++;
			}
		}
		if( doc_map.size() == 0 ){
			continue;
		}
		
		// Linea Valida
//		docid = ++next_doc;
		
		// Agregar datos al indice invertido
		for( auto frec_pair : doc_map ){
			word = frec_pair.first;
			frec = frec_pair.second;
			index.emplace(word, list<pair<unsigned int, unsigned int>>{});
			index[word].push_back( pair<unsigned int, unsigned int>(docid, frec) );
		}
		
	}
	reader.close();
	delete [] buff;
	
	cout << "Valid docs " << next_doc << ", vocabulary size : " << index.size() << "\n";
	
	cout << "Writing \"" << output << "\"\n";
	fstream writer(output, fstream::out | fstream::trunc);
	for( auto it_index : index ){
		word = it_index.first;
		writer << word << " " << it_index.second.size() << " ";
		for( auto doc_pair : it_index.second ){
			docid = doc_pair.first;
			frec = doc_pair.second;
			writer << docid << " " << frec << " ";
		}
		writer << "\n";
	}
	writer.close();
	
	cout << "End\n";
	
}

















