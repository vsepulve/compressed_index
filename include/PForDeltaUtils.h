
#ifndef _PFORDELTA_UTILS
#define _PFORDELTA_UTILS

//#include <cassert>

#include <stdint.h>
#include <iostream>
#include <cstring>

//#include "unpack.h"

#define TESTBIT(buffer, bp) ((((buffer)[(bp)>>5])>>((bp)&31))&1)

void mi_unpack_run(unsigned int* p, int loop, int tail) ;

void mi_unpack0(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack1(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack2(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack3(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack4(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack5(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack6(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack7(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack8(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack9(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack10(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack11(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack12(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack13(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack16(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack20(unsigned int* p, unsigned int* w, int BS) ;
void mi_unpack32(unsigned int* p, unsigned int* w, int BS) ;

typedef void (*pfa)(unsigned int* p, unsigned int* w, int BS);
	
class PForDeltaUtils {
public:
	PForDeltaUtils();
	~PForDeltaUtils();

	// Packs the 'b' least significant bits of the 'n' elements from input buffer at 'v' into output buffer at 'w'.
	// 'v' is a pointer to the input array of the numbers to be packed.
	// 'b' is the bit width per element in the resulting compressed array.
	// 'n' is the number of elements to be packed.
	// 'w' is a pointer to the output array.
	// Note: this function does not modify the output array pointer, it will need to be calculated and modified outside this function.
	void pack(unsigned int* v, unsigned int b, unsigned int n, unsigned int* w);

	// Reads certain bits from the input buffer.
	// 'buf' is a pointer to the input buffer.
	// 'bp' is the "bit pointer" (offset in number of bits) indicating the current position in the input buffer.
	// 'b' is the number of bits to be read starting from the bit pointer.
	// Returns the number corresponding to the 'b' bits from the input buffer.
	unsigned int readBits(unsigned int* buf, unsigned int* bp, unsigned int b);

	// Packs a number into certain bits and writes it to the output buffer.
	// 'buf' is a pointer to the output buffer.
	// 'bp' is the "bit pointer" (offset in number of bits) indicating the current position in the output buffer.
	// 'val' is the value of the number to be packed.
	// 'bits' is the bit width of the value to pack.
	void writeBits(unsigned int* buf, unsigned int* bp, unsigned int val, unsigned int bits);

	// Writes a single bit to the buffer at a bit position.
	// 'buf' is a pointer to the output buffer.
	// 'bp' is the "bit pointer" (offset in number of bits) into the output buffer at which to set the bit.
	// 'val' is the value to set the bit to.
	void setBit(unsigned char* buf, unsigned int* bp, unsigned int val);
	
	unsigned MASK[33];
	pfa unpack[17];
	
	static const int cnum[];
	
	
};

#endif /* CODING_H_ */
