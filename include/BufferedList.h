#ifndef _BUFFERED_LIST_H
#define _BUFFERED_LIST_H


class BufferedList{
	
public:
	
	BufferedList();
	virtual ~BufferedList();
	
	virtual void setBufferMemory(unsigned int *buff);
	
	virtual unsigned int getBufferMemory();
	
};
 
#endif
