
#ifndef S9INCLUDED
#define S9INCLUDED

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>

#include "InvertedList.h"
#include "BufferedList.h"
#include "Simple9Utils.h"

class Simple9BlockHeader {
	unsigned int startBlock; // position of the array where the block starts
	unsigned int lastDocId;  // last absolute docid in the block
	unsigned int numsInBlock; // amount of numbers in the block
public:
	Simple9BlockHeader() {;}
	
	void setBlockHeader(unsigned int _startBlock, unsigned int _lastDocId, unsigned int _numsInBlock)
	 {
		startBlock = _startBlock;
		lastDocId = _lastDocId;
		numsInBlock = _numsInBlock;
	 }
	 
	 unsigned int getStartBlock() {return startBlock;}
	 unsigned int getlastDocIdBlock() {return lastDocId;}
	 unsigned int getnumsInBlock() {return numsInBlock;}
	 void save(fstream *writer){
		 writer->write((char *)&startBlock, sizeof(int));
		 writer->write((char *)&lastDocId, sizeof(int));
		 writer->write((char *)&numsInBlock, sizeof(int));
	 }
	 
	 void load(fstream *saver){
		 saver->read((char *)&startBlock, sizeof(int));
		 saver->read((char *)&lastDocId, sizeof(int));
		 saver->read((char *)&numsInBlock, sizeof(int));
	 }
 };


class Simple9 : public InvertedList, public BufferedList {

private:
	
	static Simple9Utils utils;
	
	//Notar que eso es fijo para 32 bits
	static const unsigned int W = 32;
	static const unsigned int bitsW = 5;
	
 	unsigned int *docs_block_acum;
 	
	unsigned int *seq; 
	unsigned int firstDocId; // first docid in the list
	unsigned int lastDocId; // last docid in the list
	
	Simple9BlockHeader *block;
	unsigned int nBlocks;
	
	unsigned int nnums; // number of integers encoded in the sequence
	unsigned int nWords; // number of words used to encode the integers
	
	unsigned int curDocId;
	
	unsigned int lastBlock; // last block that was accessed
	
	unsigned int *buffer; // OJO, cambiar este numero
	
//	unsigned int cur_pos; // current position in the inverted list
  
	unsigned char curPosBuffer; // current position in the buffer
	
public:
	
	Simple9();
	Simple9(uint* _seq, unsigned int n);
	~Simple9();
	
	//size, in bytes, of the encoded sequence
	int getSize() const {
		return nWords * sizeof(unsigned int);
	}
	
	unsigned int uncompress(unsigned int *);
	
	unsigned int getSizeData() const {return sizeof(int) + nWords * sizeof(unsigned int);}
	
	unsigned int getSizeBlock() const {return sizeof(int) + 2 * (nBlocks+1) * sizeof(int);}
	
	//~ int numberOfDocuments() {return nnums;}
	unsigned int size() const {return nnums;}
	
	void decodeToBuffer(unsigned int posInit, unsigned int posEnd);
	
	void save(fstream *writer);
	void load(fstream *reader);
	
	// methods for DAAT query processing
	unsigned int next(unsigned int docId);
	unsigned int next();
	
	void reset();
	
	unsigned int getDocId() const {return curDocId;}
	
	bool eos() {
		return cur_pos >= nnums;
	}
	
	bool hasNext();
	
	void setBufferMemory(unsigned int *buff){
		buffer = buff;
	}
	
	unsigned int getBufferMemory(){
		//200 ?
		return 200;
	}

};
 
 #endif
