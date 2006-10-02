/***************************************************************************
 *
 *   Copyright (C) 2004 by Stephen Ord
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/denoise.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"
#include "FTransform.h"
#include "Error.h"

#include <math.h>

using namespace std;

void Pulsar::denoise (Profile* profile, int fraction) {

  int npts = profile->get_nbin();

  float * ampsarr = (float *) malloc (sizeof(float)*npts);
  float * cmplx_arr = (float *) malloc(sizeof(float)*2*npts);
  float * fft_cmplx_arr = (float *) malloc(sizeof(float)*2*npts);

  for(int i=0; i<npts; ++i){
    cmplx_arr[2*i] = profile->get_amps()[i];
    cmplx_arr[2*i+1] = 0;
  }

  FTransform::fcc1d(npts, fft_cmplx_arr, cmplx_arr);
  
  for(int i=1; i<npts/2; ++i){
    if (i> (float) npts/((float)fraction)) {
      fft_cmplx_arr[2*i]=0.0; 
      fft_cmplx_arr[2*i+1] = 0.0;
    }
    fft_cmplx_arr[2*npts-2*i] = fft_cmplx_arr[2*i];
    fft_cmplx_arr[2*npts-2*i+1] = -fft_cmplx_arr[2*i+1];
  }

  FTransform::bcc1d(npts, cmplx_arr, fft_cmplx_arr);
  for (int i = 0; i<npts ; i++) {
    ampsarr[i] = cmplx_arr[2*i]/(float)npts;
  }

  free(cmplx_arr);
  free(fft_cmplx_arr);

  profile->set_amps(ampsarr);
  free(ampsarr);  
} 
 
void Pulsar::denoise (Archive* archive, int denoise_fraction)
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::denoise all Integrations" << endl;

  for( unsigned i=0; i<archive->get_nsubint(); i++) {
    for ( unsigned j=0; j<archive->get_Integration(i)->get_nchan();j++ ) {
      for ( unsigned k=0;k<archive->get_Integration(i)->get_npol();k++) {
        denoise(archive->get_Profile (i,k,j),denoise_fraction);
      }
    }
  }
}
