#include "Pulsar/Profile.h"
#include "Error.h"
#include "fftm.h"

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

  fft::frc1d (bins, temp1.get(), get_amps());
  fft::frc1d (bins, temp2.get(), p1->get_amps());

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

  fft::bcr1d (bins, &(solution[0]), resultant.get());

  // Return the profile

  // resize(bins);
  set_amps(solution);
}

Pulsar::Profile* Pulsar::Profile::hat_profile(int nbin, float duty_cycle)
{
  if (duty_cycle >= 1.0 || duty_cycle <= 0.0)
    throw Error (InvalidParam, 
		 "Pulsar::Profile::hat_profile invalid duty cycle");
  
  if (nbin <= 0)
    throw Error (InvalidParam, "Pulsar::Profile::hat_profile invalid nbin");
  
  Pulsar::Profile* ptr = new Pulsar::Profile();

  ptr->set_centre_frequency(0.0);
  ptr->set_weight(1.0);
  ptr->set_state(Signal::None);

  ptr->resize(nbin);

  for (int i = 0; i < nbin; i++) {
    ptr->amps[i] = 0.0;
  }

  int width = int(float(nbin) * duty_cycle);

  if (nbin % 2 == 0) {

    if (width %2 != 0)
      width += 1;
    
    int start_bin = (nbin / 2) - (width / 2);
    int end_bin = start_bin + (width);

    for (int i = start_bin; i < end_bin; i++) 
      ptr->amps[i] = 1.0;
  }
  else {
    
    if (width %2 == 0)
      width += 1;
    
    int start_bin = ((nbin - 1) / 2) - ((width - 1) / 2);
    int end_bin = start_bin + width;

    for (int i = start_bin; i < end_bin; i++) 
      ptr->amps[i] = 1.0;
  }

  return ptr;
}


void Pulsar::Profile::smear (float duty_cycle)
{
  fft_convolve(hat_profile(get_nbin(), duty_cycle));
}






