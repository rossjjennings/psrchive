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
  free_mem();
}

void Pulsar::WaveletTransform::set_wavelet(string s) {
  // First char gives wavelet type, rest should give order
  if (s[0]=='H' || s[0]=='h') set_type(gsl_wavelet_haar);
  else if (s[0]=='D' || s[0]=='d') set_type(gsl_wavelet_daubechies);
  else if (s[0]=='B' || s[0]=='b') set_type(gsl_wavelet_bspline);
  else
    throw Error (InvalidParam, "Pulsar::WaveletTransform::set_wavelet",
        "Wavelet class '%c' not known", s[0]);
  int order = atoi(s.substr(1).c_str());
  set_order(order);
}

void Pulsar::WaveletTransform::free_mem() {
  if (data!=NULL) delete [] data;
  if (work!=NULL) gsl_wavelet_workspace_free(work);
  if (wave!=NULL) gsl_wavelet_free(wave);
}

void Pulsar::WaveletTransform::init_mem() {
    data = new double[npts];
    work = gsl_wavelet_workspace_alloc(npts);
    wave = gsl_wavelet_alloc(type, order);
    if ((work==NULL) || (wave==NULL)) 
      throw Error (FailedSys, "Pulsar::WaveletTransform::init_mem",
          "Error allocating working space");
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
    npts = n;
    free_mem();
    init_mem();
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

  /* TODO: If anyone wants to inverse transform something that is not a result
   * of a forward transform, we need to provide a method to set up wavelet
   * coeffs, memory, etc without having to call transform().
   */

  if ((npts==0) || (data==NULL) || (wave==NULL) || (work==NULL)) 
    throw Error (InvalidState, "Pulsar::WaveletTransform::invert",
        "invert() called before transform initialized/allocated");

  if (state!=Wavelet)
    throw Error (InvalidState, "Pulsar::WaveletTransform::invert",
        "Current state is not Wavelet");

  // Call GSL in-place transformation
  int rv = gsl_wavelet_transform_inverse(wave, data, 1, npts, work);
  if (rv!=GSL_SUCCESS)
    throw Error (FailedSys, "Pulsar::WaveletTransform::invert",
        "GSL error: %s", gsl_strerror(rv));

  // Set state
  state = Time;
}

double Pulsar::WaveletTransform::get_data(int level, int k) {

  if (data==NULL) 
    throw Error (InvalidState, "Pulsar::WaveletTransform::get_data",
        "data array not allocated");

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
  if (data==NULL) 
    throw Error (InvalidState, "Pulsar::WaveletTransform::get_data",
        "data array not allocated");
  if ((n<0) || (n>=npts)) 
    throw Error (InvalidRange, "Pulsar:WaveletTransform::get_data",
        "index %d out of range (npts=%d)", n, npts);
  return data[n];
}

double* Pulsar::WaveletTransform::get_data() {
  if (data==NULL) 
    throw Error (InvalidState, "Pulsar::WaveletTransform::get_data",
        "data array not allocated");
  return data;
}

const double* Pulsar::WaveletTransform::get_data() const {
  if (data==NULL) 
    throw Error (InvalidState, "Pulsar::WaveletTransform::get_data",
        "data array not allocated");
  return data;
}
