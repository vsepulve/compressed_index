#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>

#include "PForDelta.h"

using namespace std;

PForDeltaUtils PForDelta::utils;

PForDelta::PForDelta(){
	
	seq = NULL;
	nnums = 0; 
	data_size = 0;
	
	n_skip = 0;
	skip_list = NULL;
	skip_pointer = NULL;
	buffer = new decompress_buffer();
	
	firstDoc = 0;
	lastDoc = 0;
	currDoc = 0;
	lastBlock = 0;
	skipIdx = 0;
}

PForDelta::~PForDelta(){
	if(seq != NULL){
		delete [] seq;
	}
	if(skip_list != NULL){
		delete [] skip_list;
	}
	if(skip_pointer != NULL){
		delete [] skip_pointer;
	}
	if(buffer != NULL){
		delete buffer;
	}
}
	
PForDelta::PForDelta(unsigned int *list, unsigned int N){
	
//cout<<"PForDelta - inicio ("<<N<<" docs)\n";
	
	int real_size;
//	int block_size = 128;
	unsigned int curr_pos = 0;
	unsigned int *tmp_seq = new unsigned int[N + 1 + (N / block_size)];
	memset(tmp_seq, 0, (N + 1 + (N / block_size))*sizeof(int));
	unsigned int *tmp_seq_aux;
	unsigned int i;
	unsigned int *list_aux = list;
	unsigned int *skip_list_tmp = NULL;
	unsigned int *skip_pointer_tmp = NULL;
	
	skip_list = NULL;
	skip_pointer = NULL;
	
	unsigned int worst_skip = (N / block_size) + ((N % block_size)? 1: 0);
	
	if(worst_skip){
		skip_list_tmp = new unsigned int[worst_skip];
		memset(skip_list_tmp, 0, worst_skip*sizeof(int));
	}
	skip_pointer_tmp = new unsigned int[worst_skip+1];
	memset(skip_pointer_tmp, 0, (worst_skip+1)*sizeof(int));
	
	
	tmp_seq_aux = tmp_seq;
	nnums = N;
	
	buffer = new decompress_buffer();
	firstDoc = list[0];
	currDoc = list[0];
	skipIdx = 0;
	n_skip = 0;
	
	for(i = 0; i < N && (N - i) >= 128; i += block_size){
		real_size = BlockCompression(list_aux, tmp_seq_aux);
		
		if(n_skip){ 
			skip_list_tmp[n_skip] = skip_list_tmp[n_skip - 1];
		}
		
		for(unsigned int j = 0; j < 128; ++j){
			skip_list_tmp[n_skip] += list_aux[j];
		}
		
		skip_pointer_tmp[n_skip] = curr_pos;
		curr_pos += real_size;
		
		list_aux += block_size;
		tmp_seq_aux += real_size;
		++n_skip;
	}
	
	skip_pointer_tmp[n_skip] = curr_pos;
	
	if(n_skip){
		lastBlock = skip_list_tmp[n_skip - 1];
		lastDoc = skip_list_tmp[n_skip - 1];
	}
	else{
		//LastBlock setiado a cero porque no hay bloques. 
		lastBlock = 0;
		//lastDoc = list_aux[0];
		lastDoc = 0;
	}

	if((N - i) > 0){
		memcpy(tmp_seq_aux, list_aux, (N - i) * sizeof(int));
		
		tmp_seq_aux += (N - i);
		
		for(unsigned int j = 0; j < (N - i); ++j){
			lastDoc += list_aux[j];
		}
		
	}
	
	seq = new unsigned int[(tmp_seq_aux - tmp_seq)];
	data_size = (tmp_seq_aux - tmp_seq) * sizeof(int);
	
	memcpy(seq, tmp_seq, (tmp_seq_aux - tmp_seq) * sizeof(int));
	
	if(n_skip){
		skip_list = new unsigned int[n_skip];
		skip_pointer =  new unsigned int[n_skip + 1];
		
		memcpy(skip_list, skip_list_tmp, n_skip * sizeof(int));
		memcpy(skip_pointer, skip_pointer_tmp, (n_skip + 1) * sizeof(int));
	}
	
	delete [] tmp_seq;
	if(worst_skip){
		delete [] skip_list_tmp;
		delete [] skip_pointer_tmp;
	}
	
//cout<<"PForDelta - Fin\n";

}

int PForDelta::BlockCompression(unsigned int *input, unsigned int *output) {
	int flag = -1;
	unsigned int *w;
	for (int k = 0; flag < 0; k++) {
		w = output + 1;
		flag = pfor_encode(&w, input, k);
	}
	*output = flag;
	return w - output;
}

int PForDelta::pfor_encode(unsigned int **w, unsigned int *p, int num) {
	int i, l, n, bb, t, s;
	unsigned int m;
	int b = utils.cnum[num + 1];
	int start;
	
	unsigned int out[block_size];
	unsigned int ex[block_size];

	if (b == 32) {
		for (i = 0; i < block_size; i++) {
			(*w)[i] = p[i];
		}
		*w += block_size;
		return ((num << 12) + (2 << 10) + block_size);
	}

	// Find the largest number we're encoding.
	for (m = 0, i = 0; i < block_size; i++) {
		if (p[i] > m){
			m = p[i];
		}
	}

	if (m < 256) {
		bb = 8;
		t = 0;
	}
	else if (m < 65536) {
		bb = 16;
		t = 1;
	}
	else {
		bb = 32;
		t = 2;
	}
	
	for (start = 0, n = 0, l = -1, i = 0; i < block_size; i++) {
		if ((p[i] >= static_cast<unsigned> (1 << b)) || ((l >= 0) && (i - l == (1 << b)))) {
			if (l < 0){
				start = i;
			}
			else{
				out[l] = i - l - 1;
			}
			ex[n++] = p[i];
			l = i;
		}
		else {
			out[i] = p[i];
		}
	}
	
	if (l >= 0){
		out[l] = (1 << b) - 1;
	}
	else{
		start = block_size;
	}

	if (static_cast<double> (n) <= f_exceptions * static_cast<double> (block_size)) {
		s = ((b * block_size) >> 5);
		for (i = 0; i < s; i++) {
			(*w)[i] = 0;
		}
		utils.pack(out, b, block_size, *w);
		*w += s;

		s = ((bb * n) >> 5) + ((((bb * n) & 31) > 0) ? 1 : 0);
		for (i = 0; i < s; i++) {
			(*w)[i] = 0;
		}
		utils.pack(ex, bb, n, *w);
		*w += s;
		return ((num << 12) + (t << 10) + start);
	}

	return -1;
}

unsigned int PForDelta::uncompress(unsigned int *list){
	int real_size;
	//int block_size = 128;
	unsigned int *tmp_seq;
	unsigned int i;
	
	tmp_seq = seq;
	
	for(i = 0; i < nnums && (nnums - i) >= (unsigned int)block_size; i += block_size){
		real_size = BlockDecompression(tmp_seq, buffer->getBuffer());
		list += block_size;
		tmp_seq += real_size;
	}
	
	if((nnums - i) > 0){
		//memcpy(list, tmp_seq, (nnums - i) * sizeof(int));
		memcpy(list, tmp_seq, (nnums - i)<<2);
	}
	
	/*
	//convertir los deltas en absolutos
	for(unsigned int j = 1 ; j< nnums - i; j++){
		list[j]+=list[j-1];
	}
	*/
	
	return (nnums - i);
}

void PForDelta::reset(){
	currDoc = firstDoc;
	lastBlockIdx = 0;
	skipIdx = 0;
	cur_pos = 0;
	buffer->reset();
	if(payload_list != NULL){
		payload_list->reset();
	}
}

bool PForDelta::hasNext(){
	if(currDoc >= lastDoc) return false;
	else return true;
}

unsigned int PForDelta::next(){
	
//	cout<<"PForDelta::next - inicio\n";
	
	if(currDoc >= lastDoc){
		return currDoc = END_OF_LIST;
	}
	
//	int real_size;
	unsigned int *tmp_seq = NULL;
	
	unsigned int desired = currDoc + 1;
	
//	cout<<"PForDelta::next - desired: "<<desired<<"\n";
	if(desired > lastBlock && lastBlock != lastDoc){
//		cout<<"PForDelta::next - if...\n";
		if(!lastBlockIdx){
			currDoc = lastBlock;
			cur_pos = n_skip*128 - 1;
		}
		tmp_seq = (seq + ((n_skip)?skip_pointer[n_skip]:0));
		
		currDoc += tmp_seq[lastBlockIdx];
		++cur_pos;
		++lastBlockIdx;
		
		return currDoc;
	}
	else{
//		cout<<"PForDelta::next - else...\n";
		while(skip_list[skipIdx] < desired){
			++skipIdx;
		}
		
		if(skip_list[skipIdx] == desired){
//			cout<<"PForDelta::next - ultimo del bloque ("<<skip_list[skipIdx]<<") == desired\n";
			cur_pos = (skipIdx+1)*128 - 1;
			currDoc = desired;
			return currDoc;
		}
		
		if(buffer->getCurrBlock() == skipIdx){
//			cout<<"PForDelta::next - bloque en el buffer\n";
			//aqui no hay que hacer nada
		}
		else{
//			cout<<"PForDelta::next - bloque NO en el bufer, llenando buffer\n";
			tmp_seq = (seq + skip_pointer[skipIdx]);
			
			buffer->setCurrBlock(skipIdx);
			buffer->setCurrIdx(0);
			
//			real_size = BlockDecompression(tmp_seq, buffer->getBuffer());
			currDoc = ((!buffer->getCurrBlock())? 0 : skip_list[buffer->getCurrBlock() - 1]);
			
			cur_pos = skipIdx*128 - 1;
		}
		
//		cout<<"PForDelta::next - currDoc (pre): "<<currDoc<<"\n";
		
		while(currDoc < desired){
			currDoc += buffer->next();
			++cur_pos;
		}
		
//		cout<<"PForDelta::next - currDoc: "<<currDoc<<"\n";
		return currDoc;
	}
}

int PForDelta::BlockDecompression(unsigned int *input, unsigned int *output) {

	unsigned int *tmp = input;
	int flag = *tmp;
	int b = utils.cnum[((flag >> 12) & 15) + 1];
	int unpack_count = ((flag >> 12) & 15) + 1;
	int t = (flag >> 10) & 3;
	int start = flag & 1023;
	++tmp;
	
	int i, s;
	unsigned int x;
	(utils.unpack[unpack_count])(output, tmp, block_size);
	tmp += ((b * block_size) >> 5);
	
	switch (t) {
		case 0:
			for (s = start, i = 0; s < block_size; i++) {
				x = output[s] + 1;
				output[s] = (tmp[i >> 2] >> (24 - ((i & 3) << 3))) & 255;
				s += x;
			}
			tmp += (i >> 2);
			if ((i & 3) > 0){
				++tmp;
			}
		break;
		
		case 1:
			for (s = start, i = 0; s < block_size; i++) {
				x = output[s] + 1;
				output[s] = (tmp[i >> 1] >> (16 - ((i & 1) << 4))) & 65535;
				s += x;
			}
			tmp += (i >> 1);
			if ((i & 1) > 0){
				++tmp;
			}
		break;
		
		case 2:
			for (s = start, i = 0; s < block_size; i++) {
				x = output[s] + 1;
				output[s] = tmp[i];
				s += x;
			}
			tmp += i;
		break;
		
	}
	
	return tmp - input;
}

unsigned int PForDelta::next(unsigned int desired){
//	cout<<"PForDelta::next - desired: "<<desired<<"\n";
	int i = 0;
//	int real_size;
//	int block_size = 0;
	unsigned int first_value;
	unsigned int *tmp_seq;

	if(desired <= currDoc){
//		cout<<"PForDelta::next - desired <= currDoc ("<<currDoc<<")\n";
		if(currDoc == firstDoc){
			cur_pos=0;
		}
		return currDoc;
	}
		
	if(desired > lastDoc){
//		cout<<"PForDelta::next - desired > lastDoc ("<<lastDoc<<")\n";
		return currDoc = END_OF_LIST;
	}
		
	if(desired == lastDoc){
//		cout<<"PForDelta::next - desired == lastDoc ("<<lastDoc<<")\n";
		cur_pos = size() - 1;
		currDoc = lastDoc;
		return currDoc;
	}
		
	if(desired > lastBlock && lastBlock != lastDoc){
//		cout<<"PForDelta::next - if...\n";
		first_value = lastBlock;
		tmp_seq = (seq + ((n_skip)?skip_pointer[n_skip]:0));
		cur_pos = n_skip*128 - 1;
		while(first_value < desired){
			first_value += tmp_seq[i];
			++i;
			++cur_pos;
		}
		currDoc = first_value;
		return first_value;
	}
	else{
//		cout<<"PForDelta::next - else...\n";
		while(skip_list[skipIdx] < desired){
			++skipIdx;
		}
		
		if(skip_list[skipIdx] == desired){
//			cout<<"PForDelta::next - ultimo del bloque ("<<skip_list[skipIdx]<<") == desired\n";
			cur_pos = (skipIdx+1)*128 - 1;
			currDoc = desired;
			return currDoc;
		}
		
		if(buffer->getCurrBlock() == skipIdx){
//			cout<<"PForDelta::next - bloque en el buffer\n";
			first_value = currDoc;
		}
		else{
//			cout<<"PForDelta::next - bloque NO en el buffer\n";
			tmp_seq = (seq + skip_pointer[skipIdx]);
			
			buffer->setCurrBlock(skipIdx);
			buffer->setCurrIdx(0);
			
//			real_size = BlockDecompression(tmp_seq, buffer->getBuffer());
			first_value = ((!buffer->getCurrBlock())? 0 : skip_list[buffer->getCurrBlock() - 1]);
			
			cur_pos = skipIdx*128 - 1;
		}
		
//		cout<<"PForDelta::next - first_value: "<<first_value<<"\n";
		
		while(first_value < desired){
			first_value += buffer->next();
			++cur_pos;
		}
		
		currDoc = first_value;
//		cout<<"PForDelta::next - currDoc: "<<currDoc<<"\n";
		
		return currDoc;
	}
}

void PForDelta::save(fstream *writer){
	//
	// simple data 
	writer->write((char *)&nnums, sizeof(int));
	writer->write((char *)&data_size, sizeof(int));
	writer->write((char *)&n_skip, sizeof(int));
	writer->write((char *)&firstDoc, sizeof(int));
	writer->write((char *)&lastDoc, sizeof(int));
	writer->write((char *)&lastBlock, sizeof(int));
	
	//
	// stream data
	writer->write((char *)seq, data_size);
	if(n_skip>0){
		writer->write((char *)skip_list, n_skip * sizeof(int));
		writer->write((char *)skip_pointer, (n_skip + 1) * sizeof(int));
	}
}

void PForDelta::load(fstream *saver){
	//
	// simple data 
	saver->read((char *)&nnums, sizeof(int));
	saver->read((char *)&data_size, sizeof(int));
	saver->read((char *)&n_skip, sizeof(int));
	saver->read((char *)&firstDoc, sizeof(int));
	saver->read((char *)&lastDoc, sizeof(int));
	saver->read((char *)&lastBlock, sizeof(int));
	
	//
	// stream data
	seq = new unsigned int[data_size / sizeof(int)];
	saver->read((char *)seq, data_size);
	if(n_skip > 0){
		skip_list = new unsigned int[n_skip];
		saver->read((char *)skip_list, n_skip * sizeof(int));
		skip_pointer = new unsigned int[n_skip + 1];
		saver->read((char *)skip_pointer, (n_skip + 1) * sizeof(int));
	}
	
//	reset();
}

