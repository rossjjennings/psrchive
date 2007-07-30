/***************************************************************************
 *
 *   Copyright (C) 2002 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/Profile.h"
#include "Error.h"
#include "FTransform.h"

#include <memory>

void Pulsar::Profile::convolve (const Profile* profile)
{
  convolve (profile, -1);
}

void Pulsar::Profile::correlate (const Profile* profile)
{
  convolve (profile, 1);
}

void Pulsar::Profile::convolve (const Profile* profile, int dir)
{
  unsigned nbin = get_nbin();
  unsigned mbin = profile->get_nbin();

  vector<double> temp (nbin, 0.0);

  const float* nptr = get_amps();
  const float* mptr = profile->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++) {
    for (unsigned jbin=0; jbin < mbin; jbin++) {
      unsigned n_bin = (ibin+jbin)%nbin;
      unsigned m_bin = (mbin+dir*jbin)%mbin;
      temp[ibin] += nptr[n_bin] * mptr[m_bin];
    }
  }

  set_amps (temp);
}


void Pulsar::Profile::fft_convolve (const Profile* p1)
{
  unsigned bins = get_nbin();

  if (bins != p1->get_nbin()) {
    throw Error (InvalidParam, "Profile::fft_convolve", 
		 "profile nbin values not equal");
  }

  auto_ptr<float> temp1( new float[bins+2] );
  auto_ptr<float> temp2( new float[bins+2] );
  auto_ptr<float> resultant( new float[bins+2] );

  FTransform::frc1d (bins, temp1.get(), get_amps());
  FTransform::frc1d (bins, temp2.get(), p1->get_amps());

  // cast the float* arrays to complex<float>*
  complex<float>* c1 = (complex<float>*) temp1.get();
  complex<float>* c2 = (complex<float>*) temp2.get();
  complex<float>* r = (complex<float>*) resultant.get();

  unsigned ncomplex = bins/2+1;

  // Complex multiplication of the elements
  for (unsigned i = 0; i < ncomplex; i++)
    r[i] = c1[i]*c2[i];

  // Transform back to the time domain to get the convolved solution

  vector<float> solution;

  solution.resize(bins);

  FTransform::bcr1d (bins, &(solution[0]), resultant.get());

  // Return the profile

  // resize(bins);
  set_amps(solution);
}

