#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>

#include "PForDeltaPayloadList.h"

using namespace std;

PForDeltaUtils PForDeltaPayloadList::utils;

PForDeltaPayloadList::PForDeltaPayloadList(){
	
	seq = NULL;
	nnums = 0; 
	data_size = 0;
	
	n_skip = 0;
	skip_pointer = NULL;
	buffer = new decompress_buffer();
	
	skipIdx = 0;
	
}

PForDeltaPayloadList::~PForDeltaPayloadList(){
//	cout<<"PForDeltaPayloadList::~PForDeltaPayloadList\n";
	if(seq != NULL){
		delete [] seq;
		seq = NULL;
	}
	if(skip_pointer != NULL){
		delete [] skip_pointer;
		skip_pointer = NULL;
	}
	if(buffer != NULL){
		delete buffer;
		buffer = NULL;
	}
}


PForDeltaPayloadList::PForDeltaPayloadList(unsigned int *list, unsigned int N){

//	cout<<"PForDeltaPayloadList - inicio ("<<N<<" numeros, arr[0]: "<<list[0]<<", arr[N-1]: "<<list[N-1]<<")\n";
//	for(unsigned int i=0; i<N; ++i){
//		if(list[i]>255){
//			cout<<"Error\n";
//		}
//	}
	
	int real_size;
	unsigned int curr_pos = 0;
	unsigned int worst_seq = N + 1 + (N / block_size);
	unsigned int *tmp_seq = new unsigned int[worst_seq];
	memset(tmp_seq, 0, worst_seq * sizeof(int));
	unsigned int *tmp_seq_aux;
	unsigned int i;
	unsigned int *list_aux = list;
	unsigned int *skip_pointer_tmp = NULL;
	skip_pointer = NULL;
	
	unsigned int worst_skip = (N / block_size) + ((N % block_size)? 1: 0);
	
	skip_pointer_tmp = new unsigned int[worst_skip + 1];
	memset(skip_pointer_tmp, 0, (worst_skip+1)*sizeof(int));
	
	tmp_seq_aux = tmp_seq;
	nnums = N;
	
	buffer = new decompress_buffer();
	skipIdx = 0;
	n_skip = 0;
	
	for(i = 0; i < N && (N - i) >= (unsigned int)block_size; i += block_size){
		real_size = BlockCompression(list_aux, tmp_seq_aux);
		
		skip_pointer_tmp[n_skip] = curr_pos;
		curr_pos += real_size;
		
		list_aux += block_size;
		tmp_seq_aux += real_size;
		++n_skip;
	}
	
	skip_pointer_tmp[n_skip] = curr_pos;

	if((N - i) > 0){
//		//copia de los enteros (N-i) descomprimidos
//		memcpy(tmp_seq_aux, list_aux, (N - i) * sizeof(int));
//		tmp_seq_aux += (N - i);
		
//		cout<<"Copiando cola de "<<(N - i)<<" elementos\n";
		//copia elemento a elemento como bytes
		//Es necesario un padding a sizeof(int)
		unsigned int extra_bytes=0;
		if( ((N-i) & 0x3) != 0){
			//No es divisible por 4 => agregar 4-((N-i) & 0x3) bytes
			extra_bytes = 4-((N-i) & 0x3);
		}
//		cout<<"Extra bytes: "<<extra_bytes<<"\n";
		unsigned char *seq_bytes = (unsigned char*)tmp_seq_aux;
		for(unsigned int j=0; j<(N - i); ++j){
			if(list_aux[j] > 255){
				seq_bytes[j] = 255;
			}
			else{
				seq_bytes[j] = (unsigned char)(list_aux[j]);
			}
//			cout<<"cola["<<j<<"]: "<<(unsigned int)(seq_bytes[j])<<"\n";
		}
		tmp_seq_aux += ((N-i) + extra_bytes)/4;
//		cout<<"tmp_seq_aux avanza en "<<((N-i) + extra_bytes)/4<<" enteros\n";
	}
	
	seq = new unsigned int[(tmp_seq_aux - tmp_seq)];
	data_size = (tmp_seq_aux - tmp_seq) * sizeof(int);
	
	memcpy(seq, tmp_seq, data_size);
	
	if(n_skip){
		skip_pointer =  new unsigned int[n_skip + 1];
		memcpy(skip_pointer, skip_pointer_tmp, (n_skip + 1) * sizeof(int));
	}
	delete[] tmp_seq;
	if(worst_skip){
		delete [] skip_pointer_tmp;
	}
	
//	cout<<"PForDeltaPayloadList - fin\n";

}
	
void PForDeltaPayloadList::reset(){
	skipIdx = 0;
	buffer->reset();
}

unsigned int PForDeltaPayloadList::getPayload(unsigned int pos){
	
	//Experimentalmente puede sacarse este if si se asegura correcto
//	if(pos >= nnums){
//		return 0;
//	}
	
//	int real_size;
//	int block_size_real = 0;
//	unsigned int *tmp_seq;

	if( pos >= (n_skip << 7) ){
		//puntero casteado a char* a la posicion inicial de la cola en seq
		//se le suma la posicion indicada por el ultimo skip si habia bloques (0 si no)
		unsigned char *seq_bytes = (unsigned char*)(seq + ((n_skip)?skip_pointer[n_skip]:0));
		return seq_bytes[pos & 0x0000007f];
	}
	else{
	
		skipIdx = ((pos & 0xffffff80) >> 7);
		
		if(buffer->getCurrBlock() != skipIdx){
			//Cambiar de bloque
			
//			tmp_seq = (seq + skip_pointer[skipIdx]);
			
			buffer->setCurrBlock(skipIdx);
			buffer->setCurrIdx(0);
			
//			real_size = BlockDecompression(tmp_seq, buffer->getBuffer());
		}
		
		return buffer->at( pos & 0x0000007f );
	}
}


void PForDeltaPayloadList::save(fstream *writer){
	
	writer->write((char *)&nnums, sizeof(int));
	writer->write((char *)&data_size, sizeof(int));
	writer->write((char *)&n_skip, sizeof(int));
	
	writer->write((char *)seq, data_size);
	if(n_skip>0){
		writer->write((char *)skip_pointer, (n_skip + 1) * sizeof(int));
	}
}

void PForDeltaPayloadList::load(fstream *saver){

	saver->read((char *)&nnums, sizeof(int));
	saver->read((char *)&data_size, sizeof(int));
	saver->read((char *)&n_skip, sizeof(int));
	
	seq = new unsigned int[data_size / sizeof(int)];
	saver->read((char *)seq, data_size);
	if(n_skip > 0){
		skip_pointer = new unsigned int[n_skip + 1];
		saver->read((char *)skip_pointer, (n_skip + 1) * sizeof(int));
	}
	
//	reset();
}

int PForDeltaPayloadList::BlockCompression(unsigned int *input, unsigned int *output) {
	int flag = -1;
	unsigned int *w;
	for (int k = 0; flag < 0; k++) {
		w = output + 1;
		flag = pfor_encode(&w, input, k);
	}
	*output = flag;
	return w - output;
}

int PForDeltaPayloadList::pfor_encode(unsigned int **w, unsigned int *p, int num) {
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

int PForDeltaPayloadList::BlockDecompression(unsigned int *input, unsigned int *output) {

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


