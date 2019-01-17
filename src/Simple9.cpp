
#include "Simple9.h"

Simple9Utils Simple9::utils;

Simple9::Simple9(){
	seq = NULL;
	nnums = 0;
}

Simple9::Simple9(unsigned int *_seq, unsigned int n){
	//cout<<"Simple9::Simple9 - inicio\n";
	
	unsigned int i, maxBits, curCant, nbits;
	unsigned int *tempArray;
	
	tempArray = new unsigned int[n-1];
	memset(tempArray, 0, sizeof(int)*(n-1));
	nnums = n;
	nWords = 0;
	maxBits = 0;

	firstDocId = _seq[0]; // stores first docID separately
	curDocId = firstDocId;
	
	for (lastDocId = 0,i=0; i < n; ++i){
		lastDocId += _seq[i];
	}
	
	curCant = 0;
	
	i = 1;
	
	while (i < n) {
		nbits = utils.bits(_seq[i]);
		if (nbits >= maxBits) maxBits = utils.T[nbits];
		
		if (curCant + 1 == utils.Numbers[maxBits]) {
			// curCant+1 integers can be packed into a single word,
			// using maxBits each
			unsigned int k;
			utils.bitput(&tempArray[nWords], 28, 4, utils.Case[curCant+1]);	 
			for (k = 0; k < curCant+1; k++){
				utils.bitput(&tempArray[nWords], maxBits*k, maxBits, _seq[i-curCant+k]);
			}
			nWords++;
			curCant = 0;
			maxBits = 0;
			++i;
		}
		else if (curCant + 1 > utils.Numbers[maxBits]) {
			// adding a new integer exceeds the capacity of a word.
			// so we try to pack as much of the unpacked integers as we can
			// into a single word, and ressume the algorithm from there.
			unsigned int k;			 
			i -= curCant;
			utils.bitput(&tempArray[nWords], 28, 4, utils.Case[utils.maxNumbers[curCant]]); 
			maxBits = utils.Bits[curCant];
			for (k=0; k < utils.maxNumbers[curCant]; k++, i++){
				utils.bitput(&tempArray[nWords], maxBits*k, maxBits, _seq[i]);
			}
			++nWords;
			curCant = 0;
			maxBits = 0;
		}
		else {
			++i;
			++curCant;
		}
	}
	
	// now pack the remaining "curCant" integers
	
	unsigned int j;
	
	if (curCant > 0){
		utils.bitput(&tempArray[nWords], 28, 4, utils.Case[utils.Numbers[maxBits]]);
	} 
	for (j = 0; j < curCant; j++){
		utils.bitput(&tempArray[nWords], maxBits*j, maxBits, _seq[i-curCant+j]);
	}
	
	for (; j < utils.Numbers[(int)maxBits]; j++){
		utils.bitput(&tempArray[nWords], maxBits*j, maxBits, 0);
	}
	
	nWords += (curCant > 0);
	seq = new unsigned int[nWords];
 
	for (i = 0; i < nWords; i++){
		seq[i] = tempArray[i];
	}
	
	// now creates the blocks
	unsigned int numbersInBlock = 0; // counts the amount of numbers in a block

	nBlocks = 0; // number of blocks in the list
		
	for (i = 0; i < nWords; i++) {
		
		switch (seq[i]>>28) {
		  case 1: numbersInBlock += 1;
				  break;
		  case 2: numbersInBlock += 2;
				  break;
		  case 3: numbersInBlock += 3;
				  break;
		  case 4: numbersInBlock += 4;
				  break;
		  case 5: numbersInBlock += 5;
				  break;
		  case 6: numbersInBlock += 7;
				  break;
		  case 7: numbersInBlock += 9;
				  break;
		  case 8: numbersInBlock += 14;
				  break;
		  case 9: numbersInBlock += 28;
				  break;
		}
		
		if (numbersInBlock >= NUMBERS_IN_BLOCK) {
		  nBlocks++;
		  numbersInBlock = 0;
		}
	}
	
	if (numbersInBlock > 0){
		nBlocks += 1;
	}
	
	block = new Simple9BlockHeader[nBlocks+1];
	
	numbersInBlock = 0;
	
	unsigned int curB = 0, prevHeader = 0, curD = firstDocId;
	
	for (i = 0; i < nWords; ++i) {
		
		switch (seq[i]>>28) {
		  case 1: numbersInBlock += 1;
				  curD += (seq[i] & 0x0fffffff); 
				  break;
		  case 2: numbersInBlock += 2;
				  curD += (seq[i] & 0x00003fff) + ((seq[i]>>14) & 0x00003fff); 
				  break;
		  case 3: numbersInBlock += 3;
				  curD += (seq[i] & 0x000001ff) + ((seq[i]>>9) & 0x000001ff) + ((seq[i]>>18) & 0x000001ff);
				  break;
		  case 4: numbersInBlock += 4;
				  curD += (seq[i] & 0x0000007f) + ((seq[i]>>7) & 0x0000007f) + ((seq[i]>>14) & 0x0000007f)  
						+ ((seq[i]>>21) & 0x0000007f);	
				  break;
		  case 5: numbersInBlock += 5;
				  curD += (seq[i] & 0x0000001f) + ((seq[i]>>5) & 0x0000001f) + ((seq[i]>>10) & 0x0000001f)
						+ ((seq[i]>>15) & 0x0000001f) + ((seq[i]>>20) & 0x0000001f); 
				  break;
		  case 6: numbersInBlock += 7;
				  curD += (seq[i] & 0x0000000f) + ((seq[i]>>4) & 0x0000000f) + ((seq[i]>>8) & 0x0000000f)
						+ ((seq[i]>>12) & 0x0000000f) + ((seq[i]>>16) & 0x0000000f) + ((seq[i]>>20) & 0x0000000f)
						+ ((seq[i]>>24) & 0x0000000f); 
				  break;
		  case 7: numbersInBlock += 9;
				  curD += (seq[i] & 0x00000007) + ((seq[i]>>3) & 0x00000007) +  ((seq[i]>>6) & 0x00000007)
				  + ((seq[i]>>9) & 0x00000007) + ((seq[i]>>12) & 0x00000007) + ((seq[i]>>15) & 0x00000007) 
				  + ((seq[i]>>18) & 0x00000007) + ((seq[i]>>21) & 0x00000007) + ((seq[i]>>24) & 0x00000007); 
				  break;
		  case 8: numbersInBlock += 14;
				  curD += (seq[i] & 0x00000003) + ((seq[i]>>2) & 0x00000003) + ((seq[i]>>4) & 0x00000003)
				  + ((seq[i]>>6) & 0x00000003) + ((seq[i]>>8) & 0x00000003) + ((seq[i]>>10) & 0x00000003)
				  + ((seq[i]>>12) & 0x00000003) + ((seq[i]>>14) & 0x00000003) + ((seq[i]>>16) & 0x00000003)
				  + ((seq[i]>>18) & 0x00000003) + ((seq[i]>>20) & 0x00000003) + ((seq[i]>>22) & 0x00000003)
				  + ((seq[i]>>24) & 0x00000003) + ((seq[i]>>26) & 0x00000003); 
				  break;
		  case 9: numbersInBlock += 28;
				  curD += (seq[i] & 0x00000001) + ((seq[i]>>1) & 0x00000001) + ((seq[i]>>2) & 0x00000001)
				  + ((seq[i]>>3) & 0x00000001)+ ((seq[i]>>4) & 0x00000001)+ ((seq[i]>>5) & 0x00000001)
				  + ((seq[i]>>6) & 0x00000001)+ ((seq[i]>>7) & 0x00000001)+ ((seq[i]>>8) & 0x00000001)
				  + ((seq[i]>>9) & 0x00000001)+ ((seq[i]>>10) & 0x00000001)+ ((seq[i]>>11) & 0x00000001)
				  + ((seq[i]>>12) & 0x00000001)+ ((seq[i]>>13) & 0x00000001)+ ((seq[i]>>14) & 0x00000001)
				  + ((seq[i]>>15) & 0x00000001)+ ((seq[i]>>16) & 0x00000001)+ ((seq[i]>>17) & 0x00000001)
				  + ((seq[i]>>18) & 0x00000001)+ ((seq[i]>>19) & 0x00000001)+ ((seq[i]>>20) & 0x00000001)
				  + ((seq[i]>>21) & 0x00000001)+ ((seq[i]>>22) & 0x00000001)+ ((seq[i]>>23) & 0x00000001)
				  + ((seq[i]>>24) & 0x00000001)+ ((seq[i]>>25) & 0x00000001)+ ((seq[i]>>26) & 0x00000001)
				  + ((seq[i]>>27) & 0x00000001);
				  break;
		}
		
		if (numbersInBlock >= NUMBERS_IN_BLOCK) {
		  block[curB].setBlockHeader(prevHeader, curD, numbersInBlock);
		  prevHeader = i+1; //will start a new block, points to the beginning of it
		  numbersInBlock = 0;
		  curB++;
		}
	}

	if (numbersInBlock > 0) {
		block[curB].setBlockHeader(prevHeader, curD, numbersInBlock);
		prevHeader = i+1;
		curB++;
	}
	
	block[curB].setBlockHeader(prevHeader, 0, 0);
		
	delete [] tempArray;
	//~ cout << "Saliendo Simple9 constructor" << endl;
	
	docs_block_acum=new unsigned int[nBlocks+1];
	for(unsigned int i=0; i<nBlocks; ++i){
		docs_block_acum[i] = block[i].getnumsInBlock();
	}
	for(unsigned int i=1; i<nBlocks; ++i){
		docs_block_acum[i] += docs_block_acum[i-1];
	}
	
}
 
Simple9::~Simple9(){
	if(seq != NULL){
		delete [] seq;
	}
	if(docs_block_acum != NULL){
		delete [] docs_block_acum;
	}
}
 
void Simple9::reset() {
	cur_pos = 0;
	curDocId = firstDocId;
	lastBlock = -1;
//	start_buffer();
	if(payload_list != NULL){
		payload_list->reset();
	}
}
 
unsigned int Simple9::uncompress(unsigned int* list){
	
	unsigned int *p = list;
	unsigned int i;
	register unsigned int regAux;
	p[0] = firstDocId;
	++p;
	
	for (i=0; i<nWords; ++i) {
	
		regAux = seq[i];
		
		switch (regAux>>28) {
		  case 1: *p = regAux & 0x0fffffff; 
		  p++;
				  break;
		  case 2: *p = regAux & 0x00003fff; 
				  *(p + 1) = (regAux>>14) & 0x00003fff;
		  p += 2;
				  break;
		  case 3: *p = regAux & 0x000001ff;
				  *(p + 1) = (regAux>>9) & 0x000001ff;
				  *(p + 2) = (regAux>>18) & 0x000001ff;
		  p += 3;
				  break;
		  case 4: *p = regAux & 0x0000007f;
		  *(p + 1) = (regAux>>7) & 0x0000007f;
		  *(p + 2) = (regAux>>14) & 0x0000007f; 
		  *(p + 3) = (regAux>>21) & 0x0000007f; 
		  p += 4;
				  break;
		  case 5: *p = regAux & 0x0000001f;
				  *(p + 1) = (regAux>>5) & 0x0000001f;
				  *(p + 2) = (regAux>>10) & 0x0000001f; 
				  *(p + 3) = (regAux>>15) & 0x0000001f; 
				  *(p + 4) = (regAux>>20) & 0x0000001f; 
		  p += 5;
				  break;
		  case 6: *p = regAux & 0x0000000f; 
				  *(p + 1) = (regAux>>4) & 0x0000000f; 
				  *(p + 2) = (regAux>>8) & 0x0000000f; 
				  *(p + 3) = (regAux>>12) & 0x0000000f; 
				  *(p + 4) = (regAux>>16) & 0x0000000f; 
				  *(p + 5) = (regAux>>20) & 0x0000000f; 
				  *(p + 6) = (regAux>>24) & 0x0000000f;
		  p += 7;
				  break;
		  case 7: *p = regAux & 0x00000007;
				  *(p  + 1) = (regAux>>3) & 0x00000007;
				  *(p  + 2) = (regAux>>6) & 0x00000007; 
				  *(p  + 3) = (regAux>>9) & 0x00000007; 
				  *(p  + 4) = (regAux>>12) & 0x00000007;
				  *(p  + 5) = (regAux>>15) & 0x00000007; 
				  *(p  + 6) = (regAux>>18) & 0x00000007;
				  *(p  + 7) = (regAux>>21) & 0x00000007;
				  *(p  + 8) = (regAux>>24) & 0x00000007; 
		  p += 9;
				  break;
		  case 8: *p = regAux & 0x00000003; 
		  *(p  + 1) = (regAux>>2) & 0x00000003;
		  *(p  + 2) = (regAux>>4) & 0x00000003;
		  *(p  + 3) = (regAux>>6) & 0x00000003;
		  *(p  + 4) = (regAux>>8) & 0x00000003;
		  *(p  + 5) = (regAux>>10) & 0x00000003;
		  *(p  + 6) = (regAux>>12) & 0x00000003;
		  *(p  + 7) = (regAux>>14) & 0x00000003;
		  *(p  + 8) = (regAux>>16) & 0x00000003;
		  *(p  + 9) = (regAux>>18) & 0x00000003;
		  *(p  + 10) = (regAux>>20) & 0x00000003;
		  *(p  + 11) = (regAux>>22) & 0x00000003;
		  *(p  + 12) = (regAux>>24) & 0x00000003;
		  *(p  + 13) = (regAux>>26) & 0x00000003; 
		  p += 14;
				  break;
		  case 9: *p  = 1; *(p  + 1) = 1;  
				  *(p  + 2) = 1; *(p  + 3) = 1;  
				  *(p  + 4) = 1; *(p  + 5) = 1;  
				  *(p  + 6) = 1; *(p  + 7) = 1;  
				  *(p  + 8) = 1; *(p  + 9) = 1;  
				  *(p  + 10) = 1; *(p  + 11) = 1;  
				  *(p  + 12) = 1; *(p  + 13) = 1;  
				  *(p  + 14) = 1; *(p  + 15) = 1;  
				  *(p  + 16) = 1; *(p  + 17) = 1;  
				  *(p  + 18) = 1; *(p  + 19) = 1;  
				  *(p  + 20) = 1; *(p  + 21) = 1;  
				  *(p  + 22) = 1; *(p  + 23) = 1;  
				  *(p  + 24) = 1; *(p  + 25) = 1;  
				  *(p  + 26) = 1; *(p  + 27) = 1;  
				  p += 28;
				  break;
		}
	} 
	
//	for(unsigned int i = 1; i < nnums; ++i){
//		list[i] += list[i-1];
//	}
	
	return list - p;
  }
  
  
  
void Simple9::decodeToBuffer(unsigned int posInit, unsigned int posEnd){
	register unsigned int regAux;
	unsigned int *p = buffer;
	
	for (; posInit < posEnd; ++posInit) {
		  
		  regAux = seq[posInit];
		  
		  switch(regAux >> 28) {
			 case 1: *p= regAux & 0x0fffffff; 
			 p++;
					 break;
			 case 2: *p = regAux & 0x00003fff; 
					 *(p + 1) = (regAux>>14) & 0x00003fff;
				 p += 2;
					 break;
			 case 3: *p = regAux & 0x000001ff;
					 *(p + 1) = (regAux>>9) & 0x000001ff;
					 *(p + 2) = (regAux>>18) & 0x000001ff;
				 p += 3;
					 break;
			 case 4: *p = regAux & 0x0000007f;
				 *(p + 1) = (regAux>>7) & 0x0000007f;
					 *(p + 2) = (regAux>>14) & 0x0000007f; 
					 *(p + 3) = (regAux>>21) & 0x0000007f; 
					 p += 4;
					 break;
			 case 5: *p = regAux & 0x0000001f;
					 *(p + 1) = (regAux>>5) & 0x0000001f;
					 *(p + 2) = (regAux>>10) & 0x0000001f; 
					 *(p + 3) = (regAux>>15) & 0x0000001f; 
					 *(p + 4) = (regAux>>20) & 0x0000001f; 
			 p += 5;
					 break;
			 case 6: *p = regAux & 0x0000000f; 
					 *(p + 1) = (regAux>>4) & 0x0000000f; 
					 *(p + 2) = (regAux>>8) & 0x0000000f; 
					 *(p + 3) = (regAux>>12) & 0x0000000f; 
					 *(p + 4) = (regAux>>16) & 0x0000000f; 
					 *(p + 5) = (regAux>>20) & 0x0000000f; 
					 *(p + 6) = (regAux>>24) & 0x0000000f;
			 p += 7;
					 break;
			 case 7: *p = regAux & 0x00000007;
					 *(p  + 1) = (regAux>>3) & 0x00000007;
					 *(p  + 2) = (regAux>>6) & 0x00000007; 
					 *(p  + 3) = (regAux>>9) & 0x00000007; 
					 *(p  + 4) = (regAux>>12) & 0x00000007;
					 *(p  + 5) = (regAux>>15) & 0x00000007; 
					 *(p  + 6) = (regAux>>18) & 0x00000007;
					 *(p  + 7) = (regAux>>21) & 0x00000007;
					 *(p  + 8) = (regAux>>24) & 0x00000007; 
					 p += 9;
					 break;
			 case 8: *p = regAux & 0x00000003; 
					 *(p  + 1) = (regAux>>2) & 0x00000003;
					 *(p  + 2) = (regAux>>4) & 0x00000003;
					 *(p  + 3) = (regAux>>6) & 0x00000003;
					 *(p  + 4) = (regAux>>8) & 0x00000003;
					 *(p  + 5) = (regAux>>10) & 0x00000003;
					 *(p  + 6) = (regAux>>12) & 0x00000003;
					 *(p  + 7) = (regAux>>14) & 0x00000003;
					 *(p  + 8) = (regAux>>16) & 0x00000003;
					 *(p  + 9) = (regAux>>18) & 0x00000003;
					 *(p  + 10) = (regAux>>20) & 0x00000003;
					 *(p  + 11) = (regAux>>22) & 0x00000003;
					 *(p  + 12) = (regAux>>24) & 0x00000003;
					 *(p  + 13) = (regAux>>26) & 0x00000003; 
					 p += 14;
					 break;
			 case 9: *p  = 1; *(p  + 1) = 1; *(p  + 2) = 1; *(p  + 3) = 1;  
					 *(p  + 4) = 1; *(p  + 5) = 1; *(p  + 6) = 1; *(p  + 7) = 1;  
					 *(p  + 8) = 1; *(p  + 9) = 1; *(p  + 10) = 1; *(p  + 11) = 1;  
					 *(p  + 12) = 1; *(p  + 13) = 1; *(p  + 14) = 1; *(p  + 15) = 1;  
					 *(p  + 16) = 1; *(p  + 17) = 1; *(p  + 18) = 1; *(p  + 19) = 1;  
					 *(p  + 20) = 1; *(p  + 21) = 1; *(p  + 22) = 1; *(p  + 23) = 1;  
					 *(p  + 24) = 1; *(p  + 25) = 1; *(p  + 26) = 1; *(p  + 27) = 1;  
					 p += 28;
					 break;
		  }
		}
 }


unsigned int Simple9::next(unsigned int docId){
//	cout<<"Simple9::next - inicio (desired: "<<docId<<")\n";
	unsigned int curBlock;
	
	if(docId <= curDocId){
		return curDocId;
	}
	
	if (docId > lastDocId) {
		cur_pos = nnums;
		curDocId = END_OF_STREAM;
		return curDocId;
	}
	
	for (curBlock = lastBlock+(lastBlock==(unsigned int)(-1)); block[curBlock].getlastDocIdBlock() < docId; ++curBlock) {
//		cout<<"Simple9::next - Avance de bloque ("<<block[curBlock].getlastDocIdBlock()<<" < desired)\n";
//		cur_pos += block[curBlock].getnumsInBlock();
		cur_pos = docs_block_acum[curBlock];
		curDocId = block[curBlock].getlastDocIdBlock();
	}
	
	if (curBlock != lastBlock) {
		// block is uncompressed to the buffer
//		cout<<"Simple9::next - llenando buffer\n";
		decodeToBuffer(block[curBlock].getStartBlock(), block[curBlock+1].getStartBlock()); 	
		lastBlock = curBlock;
		curPosBuffer = 0;	
	}
	
//	cout<<"Simple9::next - avance secuencial (curDocId "<<curDocId<<")\n";
	for (; curDocId < docId; ++curPosBuffer) {
		curDocId+= buffer[curPosBuffer];
		++cur_pos;
	}
	
//	cout<<"Simple9::next - curDocId final "<<curDocId<<"\n";
	return curDocId;
 
 }
  
void Simple9::save(fstream *writer){
	//
	// simple data 
	writer->write((char *)&firstDocId, sizeof(int));
	writer->write((char *)&lastDocId, sizeof(int));
	writer->write((char *)&nBlocks, sizeof(int));
	writer->write((char *)&nnums, sizeof(int));
	writer->write((char *)&nWords, sizeof(int));
	//~ writer->write((char *)&curDocId, sizeof(int));
	//~ writer->write((char *)&lastBlock, sizeof(int));
	//~ writer->write((char *)&cur_pos, sizeof(int));
	//~ writer->write((char *)&curPosBuffer, sizeof(char));
	//~ writer->write((char *)&curPosRun, sizeof(int));
	
	//
	// stream data
	writer->write((char *)seq, nWords * sizeof(int));
	//writer->write((char *)buffer, 200 * sizeof(int)); // OJO el 200 va relacionado con el tamanio estatico que le pusieron a buffer.
	
	for(unsigned int i = 0; i < nBlocks + 1; ++i){
		block[i].save(writer);
	}
}

void Simple9::load(fstream *saver){
	//
	// simple data 
	saver->read((char *)&firstDocId, sizeof(int));
	saver->read((char *)&lastDocId, sizeof(int));
	saver->read((char *)&nBlocks, sizeof(int));
	saver->read((char *)&nnums, sizeof(int));
	saver->read((char *)&nWords, sizeof(int));
	//~ saver->read((char *)&curDocId, sizeof(int));
	//~ saver->read((char *)&lastBlock, sizeof(int));
	//~ saver->read((char *)&cur_pos, sizeof(int));
	//~ saver->read((char *)&curPosBuffer, sizeof(char));
	//~ saver->read((char *)&curPosRun, sizeof(int));
	
	//
	// stream data
	seq = new unsigned int[nWords];
	saver->read((char *)seq, nWords * sizeof(int));
	//saver->read((char *)buffer, 200 * sizeof(int)); // OJO el 200 va relacionado con el tamanio estatico que le pusieron a buffer.
	
	block = new Simple9BlockHeader[nBlocks + 1];
	for(unsigned int i = 0; i < nBlocks + 1; ++i){
		block[i].load(saver);
	}
	
	//n_docs acumulado por bloque
	docs_block_acum=new unsigned int[nBlocks+1];
	for(unsigned int i=0; i<nBlocks; ++i){
		docs_block_acum[i] = block[i].getnumsInBlock();
	}
	for(unsigned int i=1; i<nBlocks; ++i){
		docs_block_acum[i] += docs_block_acum[i-1];
	}
	
	reset();
}

bool Simple9::hasNext(){
	if(curDocId >= lastDocId){
		return false;
	}
	else{
		return true;
	}
}

unsigned int Simple9::next(){
	return next(curDocId + 1);
}



