#include "Pulsar/Profile.h"
#include "Error.h"

#include <rfftw.h>

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

  rfftw_plan forward_plan;
  rfftw_plan backward_plan;

  forward_plan = rfftw_create_plan(bins, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
  backward_plan = rfftw_create_plan(bins, FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE);

  vector<float> temp1;
  vector<float> temp2;
  vector<float> resultant;

  temp1.resize(bins);
  temp2.resize(bins);
  resultant.resize(bins);

  rfftw_one(forward_plan, (fftw_real*)get_amps(), 
	    (fftw_real*)(&(temp1[0])));
  rfftw_one(forward_plan, (fftw_real*)(p1->get_amps()), 
	    (fftw_real*)(&(temp2[0])));

  // Perform the frequency domain multiplication:
  // No complex part for the first element

  resultant[0] = temp1[0]*temp2[0];

  // Complex multiplication of the other elements

  for (unsigned int i = 1; i < (bins+1)/2; i++) {
    resultant[i] = temp1[i]*temp2[i] - temp1[bins-i]*temp2[bins-i];
    resultant[bins-i] = temp1[i]*temp2[bins-i] +temp2[i]*temp1[bins-i];
  }

  // Deal with the middle element if the array has even length

  if (bins % 2 == 0) {
    resultant[bins/2] = temp1[bins/2]*temp2[bins/2];
  }

  // Transform back to the time domain to get the convolved solution

  vector<float> solution;

  solution.resize(bins);

  rfftw_one(backward_plan, (fftw_real*)(&(resultant[0])), 
	    (fftw_real*)(&(solution[0])));

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






