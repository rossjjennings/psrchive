/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "FTransform.h"
#include "malloc16.h"

#include <math.h>

using namespace std;

/* Uses the Fourier transform to compute the derivative of an array */
void FTransform::derivative (unsigned npts, float* arr)
{
  Array16<float> cmplx_arr (2*npts);
  Array16<float> fft_cmplx_arr (2*npts);

  for (unsigned i=0; i<npts; ++i)
  {
    cmplx_arr[2*i] = arr[i];
    cmplx_arr[2*i+1] = 0;
  }

  FTransform::fcc1d(npts, &(fft_cmplx_arr[0]), &(cmplx_arr[0]));

  // zero the DC
  fft_cmplx_arr[0] = fft_cmplx_arr[1] = 0.0;
  // zero the Nyquist
  fft_cmplx_arr[npts] = fft_cmplx_arr[npts+1] = 0.0;

  for (unsigned i=1; i<npts/2; ++i)
  {
    double freq = (2.0*M_PI*i) / npts;
    double tmp = fft_cmplx_arr[2*i+1] * freq;
    fft_cmplx_arr[2*i+1] = fft_cmplx_arr[2*i] * freq;
    fft_cmplx_arr[2*i] = -tmp;
    fft_cmplx_arr[2*npts-2*i] = fft_cmplx_arr[2*i];
    fft_cmplx_arr[2*npts-2*i+1] = -fft_cmplx_arr[2*i+1];
  }

  FTransform::bcc1d(npts, &(cmplx_arr[0]), &(fft_cmplx_arr[0]));

  float norm = 1.0;
  if (FTransform::get_norm() == FTransform::unnormalized)
    norm = 1.0 / (float) npts;

  for (unsigned i=0; i<npts; ++i)
    arr[i] = cmplx_arr[2*i]*norm;
}

