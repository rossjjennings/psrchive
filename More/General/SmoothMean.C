/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/SmoothMean.h"

#include <iostream>
#include "psr_cpp.h"

/*! This method smooths the profile by setting each amplitude equal to
  the mean calculated over the region centred at that point and with
  width specified by wbin.
*/
void Pulsar::SmoothMean::smooth_data (unsigned nbin, float* output,
				      unsigned width, float* input)
{
  cerr << "Pulsar::SmoothMean::smooth_data nbin=" << nbin 
       << " width=" << width << endl;

  unsigned ibin;
  register double sum = 0.0;
  
  for (ibin=0; ibin < width; ibin++)
    sum += input[ibin];

  output[0] = sum/width;

  for (ibin=1; ibin<nbin; ibin++) {
    sum += input[ibin+width-1] - input[ibin-1];
    output[ibin] = sum/width;
  }
}
