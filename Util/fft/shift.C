#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "psrfft.h"

/* Routine to shift an array                                       */
/* Allows for arrays with npts that are not a power of 2           */
/* Replaces shiftbyfft.f                                           */

int fft_shift(int npts, float * arr, double shift){

  int i;
  double phase, cp, sp, tmp;
  float * cmplx_arr = (float *) malloc(sizeof(float)*2*npts);
  float * fft_cmplx_arr = (float *) malloc(sizeof(float)*2*npts);
  double shiftrad;

  if(npts<=1){
    fprintf(stderr, "fft_shift error - cannot fourier transform an array of %d points\n");
    return(-1);
  }  

  shiftrad = 2*M_PI*shift/(float)npts;

  for(i=0; i<npts; ++i){
    cmplx_arr[2*i] = arr[i];
    cmplx_arr[2*i+1] = 0;
  }

  fccfft1d(npts, fft_cmplx_arr, cmplx_arr);

  for(i=1; i<npts/2; ++i){
    phase = i*shiftrad;
    cp = cos(phase);
    sp = sin(phase);
    tmp = fft_cmplx_arr[2*i]*cp - fft_cmplx_arr[2*i+1]*sp;
    fft_cmplx_arr[2*i+1] = fft_cmplx_arr[2*i]*sp + fft_cmplx_arr[2*i+1]*cp;
    fft_cmplx_arr[2*i] = tmp;
    fft_cmplx_arr[2*npts-2*i] = fft_cmplx_arr[2*i];
    fft_cmplx_arr[2*npts-2*i+1] = -fft_cmplx_arr[2*i+1];
  }

  bccfft1d(npts, cmplx_arr, fft_cmplx_arr);
    
  for(i=0; i<npts; ++i)
    arr[i] = cmplx_arr[2*i]/(float)npts;

  free(cmplx_arr);
  free(fft_cmplx_arr);
  return(0);
}
