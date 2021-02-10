/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DoubleMedian.h"

#include <math.h>

#include <algorithm>
#include <numeric>
#include <utility>

Pulsar::DoubleMedian::DoubleMedian ()
{
  freq_range = 15.0; // MHz
  time_range = 600.0; // sec
}

static double weighted_median(std::vector<float> data, std::vector<float>wts)
{
  if (data.size() != wts.size()) 
    throw Error (InvalidParam, "Pulsar::DoubleMedian::weighted_median",
        "Dimension mismatch");

  const unsigned n = data.size();
  const double wtsum = std::accumulate(wts.begin(),wts.end(),0.0);

  // Special cases:
  if (n==0) return 0.0;
  if (wtsum==0.0) return 0.0;
  if (n==1) return data[0];

  std::vector< std::pair<float,float> > datwt;
  for (unsigned i=0; i<n; i++)
    datwt.push_back( std::make_pair(data[i], wts[i]) );

  std::sort(datwt.begin(), datwt.end()); // Sorts on first element

  double psum = 0.0;
  std::vector<float> bal(n,0.0);
  for (unsigned i=0; i<n; i++) {
    // Compute weight balance between elements below and above this one
    bal[i] = fabs(psum - (wtsum - psum - datwt[i].second));
    psum += datwt[i].second;
  }

  // Now find the element with best balance
  unsigned min_idx = std::min_element(bal.begin(),bal.end()) - bal.begin();
  return datwt[min_idx].first;
}

void Pulsar::DoubleMedian::smooth(std::vector<float> &smoothed,
    std::vector<float> &raw, std::vector<float> &weight,
    std::vector<float> &freqs, std::vector<float> &times)
{
  // Check array dims for consistency, determine nsub, nchan, npol
  check_dimensions(smoothed, raw, weight, freqs, times);

  std::vector<float> spectrum(nchan*npol), specwts(nchan);

  // Assumes times are sorted
  float tspan = fabs(times[nsub-1] - times[0]);
  unsigned npart = (int)(tspan/time_range) + 1;
  unsigned sub_per_part = nsub/npart;

  unsigned isub0 = 0;
  for (unsigned ipart=0; ipart<npart; ipart++) 
  {
    // Extend the last part to cover any extra samples
    if (ipart==npart-1) sub_per_part += nsub % npart;
    std::vector<float> dattmp(sub_per_part);
    std::vector<float> wttmp(sub_per_part);

    // Take median in time direction for each channel
    for (unsigned ichan=0; ichan<nchan; ichan++)
    {
      for (unsigned ipol=0; ipol<npol; ipol++)
      {
        // Grab the relevant data
        for (unsigned isub=0; isub<sub_per_part; isub++) 
        {
          dattmp[isub] = raw[idx(isub+isub0, ichan, ipol)];
          wttmp[isub] = weight[idx(isub+isub0, ichan)];
        }
        // Compute the median
        spectrum[idx(0,ichan,ipol)] = weighted_median(dattmp, wttmp);
        specwts[ichan] = std::accumulate(wttmp.begin(),wttmp.end(),0.0);
      }
    }

    // Median smooth in freq dir
    // Assumes freqs don't change very much within each time chunk
    // so that same pattern applies.
    
    for (unsigned ipol=0; ipol<npol; ipol++) 
    {
      for (unsigned ichan=0; ichan<nchan; ichan++)
      {
        dattmp.resize(0);
        wttmp.resize(0);
        float freq0 = freqs[idx(isub0,ichan)];
        for (unsigned jchan=0; jchan<nchan; jchan++)
        {
          float freq1 = freqs[idx(isub0,jchan)];
          if (fabs(freq1-freq0) < freq_range/2.0)
          {
            dattmp.push_back(spectrum[idx(0,jchan,ipol)]);
            wttmp.push_back(specwts[jchan]);
          }
        }
        float result = weighted_median(dattmp, wttmp);
        for (unsigned isub=0; isub<sub_per_part; isub++)
          smoothed[idx(isub+isub0,ichan,ipol)] = result;
      }
    }

    // Move on to next time chunk
    isub0 += sub_per_part;
  }
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::DoubleMedian::get_interface ()
{
  return new Interface (this);
}

Pulsar::DoubleMedian::Interface::Interface (DoubleMedian* instance)
{
  if (instance)
    set_instance (instance);

  add( &DoubleMedian::get_freq_range,
       &DoubleMedian::set_freq_range,
       "freq", "Frequency range (MHz)" );

  add( &DoubleMedian::get_time_range,
       &DoubleMedian::set_time_range,
       "time", "Time range (s)" );
}

