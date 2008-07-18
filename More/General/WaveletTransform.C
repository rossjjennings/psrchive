/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/WaveletTransform.h"
#include "Pulsar/Profile.h"

#include <math.h>
#include <gsl/gsl_wavelet.h>
#include <gsl/gsl_errno.h>

using namespace std;

Pulsar::WaveletTransform::WaveletTransform ()
{
  type = gsl_wavelet_daubechies;
  order = 4;
  npts = 0;
  log2_npts = 0;
  state = Empty;
  data = NULL;
  work = NULL;
  wave = NULL;
  gsl_set_error_handler_off();
}

Pulsar::WaveletTransform::~WaveletTransform ()
{
  if (data!=NULL) delete [] data;
  if (work!=NULL) gsl_wavelet_workspace_free(work);
  if (wave!=NULL) gsl_wavelet_free(wave);
}

void Pulsar::WaveletTransform::transform(const std::vector<float>& in) {
  transform(in.size(), (const float *)&in[0]);
}

void Pulsar::WaveletTransform::transform(const Pulsar::Profile* in) {
  transform(in->get_nbin(), in->get_amps());
}

void Pulsar::WaveletTransform::transform(unsigned n, const float *in) {

  // Check that n is valid
  unsigned tmp=n, tmp_log=0;
  while (tmp>1) { tmp>>=1; tmp_log++; }
  tmp = 1 << tmp_log;
  if (n % tmp) 
    throw Error (InvalidParam, "Pulsar::WaveletTransform::transform",
        "npts=%d is not a power of two", npts);
  log2_npts = tmp_log;

  // If npts has changed, need to reallocate workspace, data buf
  if (npts!=n) {
    if (data!=NULL) delete [] data;
    if (work!=NULL) gsl_wavelet_workspace_free(work);
    if (wave!=NULL) gsl_wavelet_free(wave);
    data = new double[n];
    work = gsl_wavelet_workspace_alloc(n);
    wave = gsl_wavelet_alloc(type, order);
    npts = n;
  }

  // Copy input data
  for (unsigned i=0; i<npts; i++) data[i] = in[i];

  // Call GSL in-place transformation
  int rv = gsl_wavelet_transform_forward(wave, data, 1, npts, work);
  if (rv!=GSL_SUCCESS)
    throw Error (FailedSys, "Pulsar::WaveletTransform::transform",
        "GSL error: %s", gsl_strerror(rv));

  // Set state 
  state = Wavelet;

}

void Pulsar::WaveletTransform::invert() {
  throw Error (InvalidState, "Pulsar::WaveletTransform::invert",
      "invert() not implemented yet");
}

double Pulsar::WaveletTransform::get_data(int level, int k) {

  if (state!=Wavelet)
    throw Error (InvalidState, "Pulsar:WaveletTransform::get_data",
        "data is not in Wavelet state");

  if (level==-1) return data[0]; 

  if ((level<-1) || (level>=log2_npts) || (k<0) || (k>=1<<level))
    throw Error (InvalidRange, "Pulsar::WaveletTransform::get_data",
        "index (%d,%d) out of range (log2_npts=%d)", level, k, log2_npts);

  int idx = (1<<level) + k;
  return data[idx];

}

double Pulsar::WaveletTransform::get_data(int n) {
  if ((n<0) || (n>=npts)) 
    throw Error (InvalidRange, "Pulsar:WaveletTransform::get_data",
        "index %d out of range (npts=%d)", n, npts);
  return data[n];
}
