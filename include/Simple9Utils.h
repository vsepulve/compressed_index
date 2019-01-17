#ifndef _SIMPLE9_UTILS
#define _SIMPLE9_UTILS

#define NUMBERS_IN_BLOCK 128
#define END_OF_STREAM 0xffffffff

class Simple9Utils {
	
public:
	
	//Notar que eso es fijo para 32 bits
	static const unsigned int W = 32;
	static const unsigned int bitsW = 5;
	
//	unsigned int T[33] = {1,1,2,3,4,5,7,7,9,9,14,14,14,14,14,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28};
//	// The following table is used to determine, given a number of integers, 
//	// the maximum amount of these integers that can be encoded within a single S9 word.
//	unsigned int maxNumbers[29] = {1,1,2,3,4,5,5,7,7,9,9,9,9,9,14,14,14,14,14,14,14,14,14,14,14,14,14,14,28};
//	unsigned int Bits[29]		= {28,28,14,9,7,5,5,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1};
//	unsigned int Numbers[29] = {0,28,14,9,7,5,0,4,0,3,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
//	unsigned int Case[29] = {0,1,2,3,4,5,0,6,0,7,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,9};
	
	static const unsigned int T[];
	// The following table is used to determine, given a number of integers, 
	// the maximum amount of these integers that can be encoded within a single S9 word.
	static const unsigned int maxNumbers[];
	static const unsigned int Bits[];
	static const unsigned int Numbers[];
	static const unsigned int Case[];

	unsigned int bits(unsigned int  n){
		unsigned int  b = 0;
		while (n) {
			b++;
			n >>= 1; 
		}
		return b;
	}
	
	void bitput(unsigned int *e, unsigned int p, unsigned int len, unsigned int s){
		e += p >> bitsW; 
		p &= (1<<bitsW)-1;
		if (len == W) {
			*e |= (*e & ((1<<p)-1)) | (s << p);
			if (!p){
				return;
			}
			e++;
			*e = (*e & ~((1<<p)-1)) | (s >> (W-p));
		}
		else { 
			if (p+len <= W) {
				*e = (*e & ~(((1<<len)-1)<<p)) | (s << p);
				return;
			}
			*e = (*e & ((1<<p)-1)) | (s << p);
			e++;
			len -= W-p;
			*e = (*e & ~((1<<len)-1)) | (s >> (W-p));
		}
	}
	
	Simple9Utils() {} 
	~Simple9Utils() {}
 
};

#endif




