//-*-C++-*-

#ifndef __fcomp_h_
#define __fcomp_h_

#include <stdio.h>
#include <assert.h>

/******************************************************************************/
/* these two functions are used by load and unload functions. */
/* so need to declare them here                               */ 

//  Writes out compressed data


/*****************************************************************************/
// reads in compressed data
int fcompread (unsigned nvals, float * vals, FILE * fptr, bool big_endian);

int fcompwrite(unsigned nvals,const float * vals, FILE * fptr);

#endif
