#include <math.h>
#include "Pulsar/Profile.h"
#include "Error.h"
#include "psrfft.h"

Pulsar::Profile * Pulsar::Profile::denoise (int fraction) {
  Pulsar::Profile * profile;
  profile = clone();

  float * ampsarr = (float *) malloc (sizeof(float)*get_nbin());
  float * cmplx_arr = (float *) malloc(sizeof(float)*2*get_nbin());
  float * fft_cmplx_arr = (float *) malloc(sizeof(float)*2*get_nbin());

 
  int npts = get_nbin();
  
  for(int i=0; i<npts; ++i){
    cmplx_arr[2*i] = get_amps()[i];
    cmplx_arr[2*i+1] = 0;
  }

  fccfft1d(npts, fft_cmplx_arr, cmplx_arr);
  
  for(int i=1; i<npts/2; ++i){
    if (i> (float) npts/((float)fraction)) {
      fft_cmplx_arr[2*i]=0.0; 
      fft_cmplx_arr[2*i+1] = 0.0;
    }
    fft_cmplx_arr[2*npts-2*i] = fft_cmplx_arr[2*i];
    fft_cmplx_arr[2*npts-2*i+1] = -fft_cmplx_arr[2*i+1];
  }

  bccfft1d(npts, cmplx_arr, fft_cmplx_arr);
  for (int i = 0; i<npts ; i++) {
    ampsarr[i] = cmplx_arr[2*i]/(float)npts;
  }

  
  free(cmplx_arr);
  free(fft_cmplx_arr);

  profile->set_amps(ampsarr);
  free(ampsarr);
  return profile; 
} 
  
void Pulsar::Profile::denoise_inplace (int fraction) {

  float * ampsarr = (float *) malloc (sizeof(float)*get_nbin());
  float * cmplx_arr = (float *) malloc(sizeof(float)*2*get_nbin());
  float * fft_cmplx_arr = (float *) malloc(sizeof(float)*2*get_nbin());

 
  int npts = get_nbin();
  
  for(int i=0; i<npts; ++i){
    cmplx_arr[2*i] = get_amps()[i];
    cmplx_arr[2*i+1] = 0;
  }

  fccfft1d(npts, fft_cmplx_arr, cmplx_arr);
  
  for(int i=1; i<npts/2; ++i){
    if (i> (float) npts/((float)fraction)) {
      fft_cmplx_arr[2*i]=0.0; 
      fft_cmplx_arr[2*i+1] = 0.0;
    }
    fft_cmplx_arr[2*npts-2*i] = fft_cmplx_arr[2*i];
    fft_cmplx_arr[2*npts-2*i+1] = -fft_cmplx_arr[2*i+1];
  }

  bccfft1d(npts, cmplx_arr, fft_cmplx_arr);
  for (int i = 0; i<npts ; i++) {
    ampsarr[i] = cmplx_arr[2*i]/(float)npts;
  }

  
  free(cmplx_arr);
  free(fft_cmplx_arr);

  set_amps(ampsarr);
  free(ampsarr);
  
} 
 
