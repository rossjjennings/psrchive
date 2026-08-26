/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/WaveletTransform.h"
#include "Pulsar/Profile.h"
#include "dwt_undec.h"

#include <math.h>
#include <gsl/gsl_wavelet.h>
#include <gsl/gsl_errno.h>

using namespace std;

Pulsar::WaveletTransform::WaveletTransform ()
{
  type = gsl_wavelet_daubechies;
  order = 4;
  decimate = true;
  npts = 0;
  log2_npts = 0;
  state = Empty;
  data = NULL;
  work = NULL;
  wave = NULL;
  mean = 0.0; 
  gsl_set_error_handler_off();
}

Pulsar::WaveletTransform::~WaveletTransform ()
{
  free_mem();
}

void Pulsar::WaveletTransform::set_wavelet(string s) {
  // If first char is U, don't decimate
  int idx=0;
  if (s[idx]=='U') { set_decimate(false); idx++; }
  // First char gives wavelet type, rest should give order
  if (s[idx]=='H' || s[idx]=='h') set_type(gsl_wavelet_haar);
  else if (s[idx]=='D' || s[idx]=='d') set_type(gsl_wavelet_daubechies);
  else if (s[idx]=='B' || s[idx]=='b') set_type(gsl_wavelet_bspline);
  else
    throw Error (InvalidParam, "Pulsar::WaveletTransform::set_wavelet",
        "Wavelet class '%c' not known", s[0]);
  idx++;
  int order = atoi(s.substr(idx).c_str());
  set_order(order);
}

void Pulsar::WaveletTransform::free_mem() {
  if (data!=NULL) delete [] data;
  if (work!=NULL) gsl_wavelet_workspace_free(work);
  if (wave!=NULL) gsl_wavelet_free(wave);
}

static unsigned log2i(unsigned n) {
  unsigned rv=0;
  while ((n>>rv)>1) { rv++; }
  return(rv);
}

void Pulsar::WaveletTransform::init_mem() {

  // Check that n is valid
  unsigned tmp_log = log2i(npts);
  unsigned tmp = 1 << tmp_log;
  if (npts != tmp)
    throw Error (InvalidParam, "Pulsar::WaveletTransform::init_mem",
        "npts=%u is not a power of two", npts);
  log2_npts = tmp_log;
  
  if (decimate)
  {
    data = new double[npts];
    work = gsl_wavelet_workspace_alloc(npts);
    if (work==NULL)
    throw Error (FailedSys, "Pulsar::WaveletTransform::init_mem",
                            "Error gsl_wavelet_workspace_alloc");
  }
  else
  {
    data = new double[npts*log2_npts];
  }
    
  wave = gsl_wavelet_alloc(type, order);

  if (wave==NULL) 
    throw Error (FailedSys, "Pulsar::WaveletTransform::init_mem",
        "Error gsl_wavelet_alloc");
}

void Pulsar::WaveletTransform::transform(const std::vector<float>& in) {
  transform(in.size(), (const float *)&in[0]);
}

void Pulsar::WaveletTransform::transform(const Pulsar::Profile* in) {
  transform(in->get_nbin(), in->get_amps());
}

void Pulsar::WaveletTransform::transform(unsigned n, const float *in) {

  // If npts has changed, need to reallocate workspace, data buf
  if (npts!=n) {
    npts = n;
    free_mem();
    init_mem();
  }

  if (decimate) {

    // Standard (decimated) wavelet transform

    // Copy input data
    for (unsigned i=0; i<npts; i++) data[i] = in[i];

    // Call GSL in-place transformation
    int rv = gsl_wavelet_transform_forward(wave, data, 1, npts, work);
    if (rv!=GSL_SUCCESS)
      throw Error (FailedCall, "Pulsar::WaveletTransform::transform",
          "GSL error: %s", gsl_strerror(rv));

  } else {

    // Undecimated (translation-invariant) transform

    // Copy input data
    // UDWT doesn't deal well with mean signal level, so
    // we subtract it and add it back in when doing the 
    // inverse transform.
    double *tmp = new double[npts];
    mean = 0.0;
    for (unsigned i=0; i<npts; i++) mean += in[i];
    mean /= (double)npts;
    for (unsigned i=0; i<npts; i++) tmp[i] = in[i] - mean;

    int rv = dwt_undec_transform(tmp, data, npts, wave);
    if (rv!=(int)log2_npts)
      throw Error (FailedCall, "Pulsar::WaveletTransform::transform",
          "dwt_undec_transform failed (rv=%d)", rv);

    delete [] tmp;
    
  }

  // Set state 
  state = Wavelet;

}

void Pulsar::WaveletTransform::invert() {

  /* TODO: If anyone wants to inverse transform something that is not a result
   * of a forward transform, we need to provide a method to set up wavelet
   * coeffs, memory, etc without having to call transform().
   */

  if ((npts==0) || (data==NULL) || (wave==NULL) || (decimate && work==NULL)) 
    throw Error (InvalidState, "Pulsar::WaveletTransform::invert",
        "invert() called before transform initialized/allocated");

  if (state!=Wavelet)
    throw Error (InvalidState, "Pulsar::WaveletTransform::invert",
        "Current state is not Wavelet");

  if (decimate) {

    // Standard DWT

    // Call GSL in-place transformation
    int rv = gsl_wavelet_transform_inverse(wave, data, 1, npts, work);
    if (rv!=GSL_SUCCESS)
      throw Error (FailedCall, "Pulsar::WaveletTransform::invert",
          "GSL error: %s", gsl_strerror(rv));

  } else {

    // Undecimated DWT

    double *tmp = new double[npts];

    int rv = dwt_undec_inverse(data, tmp, npts, wave);
    if (rv!=(int)log2_npts)
      throw Error (FailedCall, "Pulsar::WaveletTransform::invert",
          "dwt_undec_inverse failed (rv=%d)", rv);

    // Copy result
    for (unsigned i=0; i<npts; i++) data[i] = tmp[i] + mean;

    delete [] tmp;

  }

  // Set state
  state = Time;
}

unsigned Pulsar::WaveletTransform::get_ncoeff(int level) const {
  if (level==-1) return 1;
  if (decimate)
    return 1<<level;
  else
    return npts;
}

double Pulsar::WaveletTransform::get_data(int level, unsigned k) {

  if (data==NULL) 
    throw Error (InvalidState, "Pulsar::WaveletTransform::get_data",
        "data array not allocated");

  if (state!=Wavelet)
    throw Error (InvalidState, "Pulsar:WaveletTransform::get_data",
        "data is not in Wavelet state");

  if ((level<-1) || (level>=(int)log2_npts))
   throw Error (InvalidRange, "Pulsar::WaveletTransform::get_data",
      "level %d out of range (log2_npts=%d)", level, log2_npts); 

  if (k>=get_ncoeff(level))
    throw Error (InvalidRange, "Pulsar::WaveletTransform::get_data",
      "index %d out of range (level=%d, log2_npts=%d, npts=%u, decimate=%s)",
      k, level, log2_npts, npts, decimate?"true":"false");

  if (decimate) { 

    if (level==-1) return data[0]; 

    int idx = (1<<level) + k;
    return data[idx];

  } else {

    if (level==-1) return mean;

    unsigned idx = (unsigned)((log2_npts - level) - 1)*npts + k;
    return data[idx];

  }

}

double Pulsar::WaveletTransform::get_data(int n) {
  if (data==NULL) 
    throw Error (InvalidState, "Pulsar::WaveletTransform::get_data",
        "data array not allocated");
  unsigned max_n = decimate ? npts : (unsigned)log2_npts*npts;
  if ((n<0) || ((unsigned)n>=max_n))
    throw Error (InvalidRange, "Pulsar:WaveletTransform::get_data",
        "index %d out of range (max_n=%u)", n, max_n);
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
