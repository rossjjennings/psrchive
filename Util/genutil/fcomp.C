#include <stdio.h>
#include <assert.h>
#include <math.h>

#include <iostream>

#include "convert_endian.h"
#include "fcomp.h"

/******************************************************************************/
/* these two functions are used by load and unload functions. */
/* so need to declare them here                               */ 

//  Writes out compressed data

static unsigned short int * packed_buf = NULL;
static unsigned packed_buf_sz = 0;

/*****************************************************************************/
// reads in compressed data
int fcompread (unsigned nvals, float * vals, FILE * fptr, bool big_endian)
{
  if (packed_buf_sz < nvals)  {
    if (packed_buf) delete [] packed_buf; packed_buf = NULL;
    packed_buf = new unsigned short int [nvals]; assert (packed_buf != NULL);
    packed_buf_sz = nvals;
  }

  float offset,scale;
  if (fread(&scale,sizeof(float),1,fptr) < 1)  {
    perror ("fcompread: fail fread scale");
    return -1;
  }
  if (fread(&offset,sizeof(float),1,fptr) < 1)  {
    perror ("fcompread: fail fread offset");
    return -1;
  }
  if (fread(packed_buf,sizeof(unsigned short int),nvals,fptr) < nvals)  {
    perror ("fcompread: fail fread nvals");
    fprintf(stderr, "could not read %d bins from archive\n", nvals);
    return -1;
  }
  if (big_endian) {
    FromBigEndian (scale);
    FromBigEndian (offset);
    N_FromBigEndian (nvals, packed_buf);
  }
  else {
    FromLittleEndian   (scale);
    FromLittleEndian   (offset);
    N_FromLittleEndian (nvals, packed_buf);
  }

  if(scale==0 || isnan(scale) ){
    fprintf(stderr, "fcompread error - scale==0, indicating imminent divsion by zero\n");
    return(-1);
  }

  for (unsigned k=0;k<nvals;k++)
    vals[k]= (((float) packed_buf[k]) / scale) + offset ;

  if (ferror(fptr))  {
    perror ("fcompread:");
    return -1;
  }

  return 0;
}

int fcompwrite(unsigned nvals,const float * vals, FILE * fptr)
{
  if (!vals) {
    cerr << "fcompwrite: invalid vals" << endl;
    return -1;
  }

  if (packed_buf_sz < nvals)  {
    if (packed_buf) delete [] packed_buf; packed_buf = NULL;
    packed_buf = new unsigned short int [nvals]; assert (packed_buf != NULL);
    packed_buf_sz = nvals;
  }

  assert (sizeof (unsigned short int) == 2);

  float two16 = 64*1024 - 1;              // just to be safe

  float xmin,xmax;
  xmin = xmax = vals[0];

  unsigned k=0;
  for (k=0; k<nvals; k++) {
    if (xmin > vals[k]) xmin = vals[k];
    if (xmax < vals[k]) xmax = vals[k];
  }

  float scale;
  if (xmin != xmax)
    scale = (float) two16 / (xmax - xmin);
  else
    scale = 1.0;

  if (scale == 0.0 || isnan(scale)) {
    cerr << "fcompwrite: XMIN=" << xmin << " XMAX=" << xmax 
	 << " SCALE=" << scale << endl;
    return -1;
  }

  float offset = xmin;

  for (k=0; k<nvals; k++)
    packed_buf[k]= (unsigned short int)((vals[k] - offset) * scale);

  ToBigEndian (scale);
  ToBigEndian (offset);
  N_ToBigEndian (nvals, packed_buf);

  fwrite (&scale,  sizeof(scale),  1,fptr);
  fwrite (&offset, sizeof(offset), 1,fptr);
  fwrite (packed_buf,  nvals*sizeof(unsigned short int), 1,fptr);

  if (ferror(fptr))  {
    perror ("fcompwrite:");
    return -1;
  }

  return 0;
}
