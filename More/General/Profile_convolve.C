#include <rfftw.h>

#include "Pulsar/Profile.h"
#include "Error.h"

void Pulsar::Profile::fft_convolve(Profile* p1, Profile* p2)
{
  unsigned int bins = p1 -> get_nbin();

  if (p1 -> get_nbin() != p2 -> get_nbin()) {
    throw Error (InvalidParam, "Profile::fft_convolve", "profile nbin values not equal");
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

  rfftw_one(forward_plan, p1 -> get_amps(), temp1.begin());
  rfftw_one(forward_plan, p2 -> get_amps(), temp2.begin());

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

  rfftw_one(backward_plan, resultant.begin(), solution.begin());

  // Return the profile

  resize(bins);
  set_amps(solution.begin());
}


void Pulsar::Profile::hat_profile(int bin_number, int width)
{
  vector<float> new_amps(bin_number, 1);

  resize(bin_number);
  set_amps(new_amps.begin());

  if (bin_number % 2 == 0 && width % 2 == 0) {
    for (int i = (width/2); i < (bin_number)-(width/2); i++) 
      amps[i] = 0.0;
  }
  else {
    throw Error (InvalidParam, "Profile::hat_profile",
		 "Width not an even division of bins");
  }
}









