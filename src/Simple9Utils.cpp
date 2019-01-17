
#include "Simple9Utils.h"

const unsigned int Simple9Utils::T[33] = {1,1,2,3,4,5,7,7,9,9,14,14,14,14,14,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28};
// The following table is used to determine, given a number of integers, 
// the maximum amount of these integers that can be encoded within a single S9 word.
const unsigned int Simple9Utils::maxNumbers[29] = {1,1,2,3,4,5,5,7,7,9,9,9,9,9,14,14,14,14,14,14,14,14,14,14,14,14,14,14,28};
const unsigned int Simple9Utils::Bits[29]		= {28,28,14,9,7,5,5,4,4,3,3,3,3,3,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1};
const unsigned int Simple9Utils::Numbers[29] = {0,28,14,9,7,5,0,4,0,3,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
const unsigned int Simple9Utils::Case[29] = {0,1,2,3,4,5,0,6,0,7,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,9};


