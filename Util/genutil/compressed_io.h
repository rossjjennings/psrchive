#include <stdio.h>
#include <vector>

#include "psr_cpp.h"

// ///////////////////////////////////////////////////////////////////////
// fwrite_compressed
//    writes out a vector of floats to file as an array of unsigned short
//    writes:
//            float offset;
//            float ratio;
//            float nelements;
//            unsigned short elements [nelements];
//    where:
//            vals[N] = offset + elements[N] * ratio;

int fwrite_compressed (FILE* fptr, vector<float>& vals);

// ///////////////////////////////////////////////////////////////////////
// fread_compressed
//    reads in a vector of floats written by the above function...
//    set 'swapendian' to true if the file was written on a machine of 
//    opposite endian.
int fread_compressed (FILE* fptr, vector<float>* vals, bool swapendian=false,
		      bool correct_uint64_bug = false);
