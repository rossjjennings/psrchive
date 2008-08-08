/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/WaveletSmooth.h"
#include "Pulsar/WaveletTransform.h"
#include "Pulsar/Profile.h"

#include <algorithm>

using namespace std;

Pulsar::WaveletSmooth::WaveletSmooth ()
{
  // These seem like good defaults
  thresh = Hard;
  set_wavelet(gsl_wavelet_daubechies, 8);
}

Pulsar::WaveletSmooth::~WaveletSmooth ()
{
}

void Pulsar::WaveletSmooth::set_wavelet(const gsl_wavelet_type *t, int order) {
  wt.set_type(t);
  wt.set_order(order);
}

void Pulsar::WaveletSmooth::transform(Pulsar::Profile *prof) {

  // Do wavelet transform
  wt.transform(prof);

  // Calculate noise level from median of highest level
  // wavelet coeffs
  std::vector<double> wc;
  wc.resize(prof->get_nbin()/2);
  for (unsigned i=0; i<wc.size(); i++) 
    wc[i] = fabs(wt.get_data(wt.get_log2_npts()-1,i));
  std::nth_element(wc.begin(), wc.begin() + wc.size()/2, wc.end());
  sigma = (1.0/0.6745)*wc[wc.size()/2];

  // Threshold wavelet coeffs
  ncoeff = prof->get_nbin();
  cutoff = 1.3*sqrt(2.0 * log((double)ncoeff));
  for (unsigned i=1; i<prof->get_nbin(); i++) {
    if (thresh==Hard) wt.get_data()[i] = thresh_hard(wt.get_data(i));
    else if (thresh==Soft) wt.get_data()[i] = thresh_soft(wt.get_data(i));
    else 
      throw Error (InvalidState, "Pulsar::WaveletSmooth::transform",
          "Unrecognized threshold setting");
    if (wt.get_data(i)==0.0) ncoeff--;
  }

  // Inverse transform, replace input data
  wt.invert();
  for (unsigned i=0; i<prof->get_nbin(); i++) 
    prof->get_amps()[i] = wt.get_data(i);

}

double Pulsar::WaveletSmooth::thresh_hard(double in) {
  if (fabs(in) > cutoff*sigma) return in;
  else return 0.0;
}

double Pulsar::WaveletSmooth::thresh_soft(double in) {
  if (fabs(in) > cutoff*sigma) {
    if (in>0.0) return in - cutoff*sigma;
    else return in + cutoff*sigma;
  } else 
    return 0.0;
}

