#include "Pulsar/SmoothMedian.h"

#include <algorithm>
#include <iostream>
#include "psr_cpp.h"

/*! This method smooths the profile by setting each amplitude equal to
  the median calculated over the region centred at that point and with
  width specified by width.
*/
void Pulsar::SmoothMedian::smooth_data (unsigned nbin, float* output,
					unsigned width, float* input)
{
  cerr << "Pulsar::SmoothMedian::smooth_data nbin=" << nbin 
       << " width=" << width << endl;

  unsigned middle = width/2;
  vector<float> window (width);

  for (unsigned ibin=0; ibin<nbin; ibin++) {

    for (unsigned jbin=0; jbin<width; jbin++)
      window[jbin] = input[ibin+jbin];

    sort (window.begin(), window.end());
    output[ibin] = window[middle];

  }
}
