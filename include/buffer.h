#ifndef _DECOM_BUFFER_
#define _DECOM_BUFFER_

#include <cstdio>
#include <iostream>

class decompress_buffer{
	unsigned int currBlock;	
	unsigned int currIdx;
	unsigned int firstIdRun;
	unsigned int lastIdRun;
	bool runFlag;
	
	public:
	
	unsigned int *buff;
	
	decompress_buffer(){
		buff = NULL;
		currBlock = -1;	
		currIdx = 0;
		runFlag = false;
		lastIdRun = 0;
	}	
	
	~decompress_buffer(){
		//el buff es prestado por el llamador
	}
	
	inline unsigned int getCurrBlock(){
		return currBlock;
	}
	
	inline void setCurrBlock(unsigned int value){
		currBlock = value;
	}
	
	inline void setCurrIdx(unsigned int value){
		currIdx = value;
	}
	
	inline void setBuffer(unsigned int *value){
		memcpy(buff, value, 128 * sizeof(int));
	}
	
	inline unsigned int next(){
		return buff[currIdx++];
	}
	
	inline unsigned int at(unsigned int pos){
		return buff[pos];
	}
	
	inline void setRun(unsigned int first, unsigned int last){
		firstIdRun = first;
		lastIdRun = last;
		runFlag = true;
	}
	
	inline void unsetRun(){
		firstIdRun = 0;
		lastIdRun = 0;
		runFlag = false;
	}
	
	inline bool inRun(){
		return runFlag;
	}
	
	inline unsigned int maxRun(){
		return lastIdRun;
	}	
	
	inline unsigned int *getBuffer(){
		return buff;
	}
	
	inline unsigned int getLastId(){
		return lastIdRun;
	}
	
	inline unsigned int getFirstId(){
		return firstIdRun;
	}
	
	inline void reset(){
		currBlock = -1;	
		currIdx = 0;
		unsetRun();
	}
};

#endif
