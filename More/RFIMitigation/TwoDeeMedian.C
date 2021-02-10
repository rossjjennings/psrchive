/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TwoDeeMedian.h"

#include <math.h>

#include <algorithm>
#include <numeric>
#include <utility>

Pulsar::TwoDeeMedian::TwoDeeMedian ()
{
  med_nchan = 5;
  med_nsubint = 15;

  freq_range = time_range = 0.0;
}



void Pulsar::TwoDeeMedian::smooth(std::vector<float> &smoothed,
    std::vector<float> &raw, std::vector<float> &weight,
    std::vector<float> &freqs, std::vector<float> &times)
{
  // Check array dims for consistency, determine nsub, nchan, npol
  check_dimensions(smoothed, raw, weight, freqs, times);

  std::vector<float> data (med_nchan * med_nsubint, 0.0);

  unsigned half_nchan = med_nchan / 2;
  unsigned half_nsubint = med_nsubint / 2;
  
  for (unsigned ipol=0; ipol < npol; ipol++)
  {
    for (unsigned isub=0; isub < nsub; isub++)
    {
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
	if (weight[idx(isub, ichan)] == 0.0)
	  continue;

	unsigned start_subint = 0;
	unsigned end_subint = isub + half_nsubint + 1;

	if (isub > half_nsubint)
	  start_subint = isub - half_nsubint;
	if (end_subint > nsub)
	  end_subint = nsub;

	unsigned start_chan = 0;
	unsigned end_chan = ichan + half_nchan + 1;
	if (ichan > half_nchan)
	  start_chan = ichan - half_nchan;
	if (end_chan > nchan)
	  end_chan = nchan;

	unsigned valid = 0;

#if _DEBUG
	cerr << "isub=" << isub << " ichan=" << ichan
	     << " ssub=" << start_subint << " esub=" << end_subint
	     << " sch=" << start_chan << " ech=" << end_chan << endl;
#endif
	
	for (unsigned jsub=start_subint; jsub < end_subint; jsub++)
	{
	  for (unsigned jchan=start_chan; jchan < end_chan; jchan++)
	  {
	    if (weight[idx(jsub, jchan)] == 0.0)
	      continue;

	    data[valid] = raw[idx(jsub, jchan, ipol)];
	    valid ++;
	  }
	}

	unsigned index = idx(isub, ichan, ipol);
	double local_median = raw[index];
	
	if (valid > 2)
        {
	  unsigned mid = valid / 2;
	  std::nth_element(data.begin(), data.begin()+mid, data.begin()+valid);
	  local_median = data[mid];
	}

	smoothed[index] = local_median;
      }
    }
  }
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::TwoDeeMedian::get_interface ()
{
  return new Interface (this);
}

Pulsar::TwoDeeMedian::Interface::Interface (TwoDeeMedian* instance)
{
  if (instance)
    set_instance (instance);

  add( &TwoDeeMedian::get_nchan,
       &TwoDeeMedian::set_nchan,
       "nchan", "Number of frequency channels");

  add( &TwoDeeMedian::get_nsubint,
       &TwoDeeMedian::set_nsubint,
       "nsubint", "Number of sub-integrations");

#if IMPLEMENTED
  add( &TwoDeeMedian::get_freq_range,
       &TwoDeeMedian::set_freq_range,
       "freq", "Frequency range (MHz)" );

  add( &TwoDeeMedian::get_time_range,
       &TwoDeeMedian::set_time_range,
       "time", "Time range (s)" );
#endif
}

