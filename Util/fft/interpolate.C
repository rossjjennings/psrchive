/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "interpolate.h"
#include "FTransform.h"
#include "malloc16.h"

#include <assert.h>
using namespace std;

//! Use zero-padded Fourier transform to interpolate between samples
void fft::interpolate (complex<float>* dest, unsigned n_dest, const complex<float>* src, unsigned n_src)
{
  if (n_src >= n_dest)
    throw Error (InvalidParam, "fft::interpolate",
                "n_src=%u >= n_dest=%u", n_src, n_dest);

  assert (dest != nullptr);
  assert (src != nullptr);
  assert (n_dest > 0);
  assert (n_src > 0);

  Array16<float> dom2 (n_dest * 2);
  auto c_dom2 = reinterpret_cast<complex<float>*>(dom2.get());

  FTransform::fcc1d (n_src, dom2, reinterpret_cast<const float*>(src));

  // shift the negative frequencies to the end
  unsigned npt2 = n_src/2;
  for (unsigned ipt=0; ipt<npt2; ipt++)
    c_dom2[n_dest-1-ipt] = c_dom2[n_src-1-ipt];
  
  // zero pad the rest
  unsigned end = n_dest - npt2;
  for (unsigned ipt=npt2; ipt<end; ipt++)
    c_dom2[ipt] = 0;

  FTransform::bcc1d (n_dest, reinterpret_cast<float*>(dest), dom2);

  float scalefac = 1.0;

  if (FTransform::get_norm() == FTransform::unnormalized)
    scalefac = 1.0 / float(n_src);
  else
    scalefac = float(n_dest) / float(n_src);

  for (unsigned ipt=0; ipt < n_dest; ipt++)
    dest[ipt] *= scalefac;
}