#ifndef _PFORDELTA_PAYLOAD_LIST
#define _PFORDELTA_PAYLOAD_LIST

#include "PForDeltaUtils.h"
#include "PayloadList.h"
#include "BufferedList.h"
#include "buffer.h"

//#define BLOCK_SIZE 128

class PForDeltaPayloadList : public PayloadList, public BufferedList{

protected: 
	
	//datos
	unsigned int *seq;
	//numero de docs
	unsigned int nnums;
	//tamaño de seq en bytes
	unsigned int data_size;
	
	unsigned int n_skip;
	unsigned int *skip_pointer;
	
	unsigned int skipIdx; 
	
	decompress_buffer *buffer;
	
	int BlockDecompression(unsigned int* input, unsigned int* output);
	
	int BlockCompression(unsigned int* input, unsigned int* output);
	int pfor_encode(unsigned int **w, unsigned int *p, int num);
	
	static PForDeltaUtils utils;
	static const int block_size = 128;
	static constexpr float f_exceptions = 0.1;
	
public:

	PForDeltaPayloadList();
	PForDeltaPayloadList(unsigned int *, unsigned int);  
	PForDeltaPayloadList(list<unsigned int> *lista_original);
	virtual ~PForDeltaPayloadList();
	
	// 1 int para data_size, mas los bytes
	unsigned int getSizeData() const {
		return sizeof(int) + data_size;
	}
	// 1 int para n_skip, x2 pues se guarda el ultimo doc de cada bloque, y las posiciones para saltar
	unsigned int getSizeBlock() const {
		//el ultimo entero es porque skip_pointer usa 1 entero ademas de n_skip (para la cola)
    	return sizeof(int) + n_skip*2*sizeof(int) + sizeof(int);
	}
	
	virtual void reset();
	virtual unsigned int size() const {return nnums;}
	virtual unsigned int getPayload(unsigned int pos);
	
	virtual void save(fstream *writer);
	virtual void load(fstream *reader);
	
	void setBufferMemory(unsigned int *buff){
		buffer->buff = buff;
	}
	
	unsigned int getBufferMemory(){
		return 128;
	}
	
	//Metodos para entrega de datos directos para la clase Cover
	unsigned int *getData(){
		return seq;
	}
	unsigned int getDataSize(){
		return data_size;
	}
	unsigned int *getBlocks(){
		return skip_pointer;
	}
	unsigned int getNumBlocks(){
		return n_skip;
	}
	
	
};
 
#endif
