#ifndef _PFORDELTA
#define _PFORDELTA

#include <typeinfo>

#include "PForDeltaUtils.h"
#include "InvertedList.h"
#include "BufferedList.h"
#include "buffer.h"


class PForDelta : public InvertedList, public BufferedList{
	
	//datos
	unsigned int *seq;
	//numero de docs
	unsigned int nnums;
	//tamaño de seq en bytes
	unsigned int data_size;
	
	//numero de bloques
	unsigned int n_skip;
	//ultimo doc acumulado de cada bloque
	unsigned int *skip_list;
	//posicion en seq
	unsigned int *skip_pointer;
	
	unsigned int firstDoc;
	unsigned int lastDoc;
	unsigned int currDoc;
	unsigned int lastBlock;
	unsigned int lastBlockIdx;
	unsigned int skipIdx; 
	
	decompress_buffer *buffer;
	
	int BlockDecompression(unsigned int* input, unsigned int* output);
	
	int BlockCompression(unsigned int* input, unsigned int* output);
	int pfor_encode(unsigned int **w, unsigned int *p, int num);
	
	static PForDeltaUtils utils;
	static const int block_size = 128;
	static constexpr float f_exceptions = 0.1;
	
public:
	
	PForDelta();
	PForDelta(unsigned int *, unsigned int);  
	PForDelta(list<unsigned int> *lista_original);
	~PForDelta();
	
	unsigned int uncompress(unsigned int *);
	unsigned int size() const {return nnums;}
	// 1 int para data_size, mas los bytes
	unsigned int getSizeData() const {
		return (sizeof(int) + data_size);
	}
	// 1 int para n_skip, x2 pues se guarda el ultimo doc de cada bloque, y las posiciones para saltar
	unsigned int getSizeBlock() const {
		//el ultimo entero es porque skip_pointer usa 1 entero ademas de n_skip (para la cola)
		return sizeof(int) + n_skip*2*sizeof(int) + sizeof(int);
	}
	unsigned int getLastDoc() const {return lastDoc;}
	unsigned int getDocId() const {return currDoc;}
	void reset();
	//~ bool eos();
	unsigned int next(unsigned int);
	//unsigned int next(){return next(currDoc + 1);}
	unsigned int next();
	bool hasNext();
	void save(fstream *writer);
	void load(fstream *reader);
	void start_buffer();
	void destroy_buffer();
	unsigned int isRun(){ return buffer->inRun();}
	unsigned int *getBuffer(){ return buffer->getBuffer();}
	
	void setBufferMemory(unsigned int *buff){
		buffer->buff=buff;
	}
	
	unsigned int getBufferMemory(){
		return 128;
	}
	
};
 
#endif
