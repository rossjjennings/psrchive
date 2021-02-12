/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/InterQuartileRange.h"

#include <algorithm>
#include <numeric>
#include <utility>

// #define _DEBUG 1

using namespace std;

Pulsar::InterQuartileRange::InterQuartileRange ()
{
  set_cutoff_threshold (1.5);
}

unsigned Pulsar::InterQuartileRange::update_mask (std::vector<float> &mask, 
						  std::vector<float> &stat,
						  std::vector<float> &model,
						  unsigned nsubint,
						  unsigned nchan,
						  unsigned npol)
{
  // number of values tested per polarization
  const unsigned ntest = nsubint * nchan;

#if _DEBUG
  cerr << "InterQuartileRange::update_mask nsubint=" << nsubint
       << " nchan=" << nchan << " npol=" << npol << " ntot=" << ntot 
       << " stat.sz=" << stat.size() << " model.sz=" << model.size()
       << " mask.sz=" << mask.size() << endl;
#endif

  unsigned too_high = 0;
  unsigned too_low = 0;

  for (unsigned ipol=0; ipol < npol; ipol++)
  { 
    std::vector<float> data (ntest, 0.0);
    unsigned valid = 0;

    for (unsigned isub=0; isub<nsubint; isub++)
    {
      for (unsigned ichan=0; ichan<nchan; ichan++)
      {
        unsigned idat = isub*nchan*npol + ichan*npol + ipol;
        unsigned imask = isub*nchan + ichan;

        if (mask[imask] == 0.0)
          continue;

        data[valid] = stat[idat];

        if (model.size() == stat.size())
          data[valid] -= model[idat];

        valid ++;
      }
    }

    assert (valid <= ntest);
    data.resize(valid);

    unsigned iq1 = valid/4;
    unsigned iq3 = (valid*3)/4;

#ifdef _DEBUG
    cerr << "iQ1=" << iq1 << " iQ3=" << iq3 << endl;
#endif

    std::sort (data.begin(), data.begin()+valid);
    double Q1 = data[ iq1 ];
    double Q3 = data[ iq3 ];
  
    double IQR = Q3 - Q1;

    for (unsigned isub=0; isub<nsubint; isub++)
    {
      for (unsigned ichan=0; ichan<nchan; ichan++)
      {
        unsigned idat = isub*nchan*npol + ichan*npol + ipol;
        unsigned imask = isub*nchan + ichan;

        if (mask[imask] == 0.0)
          continue;

        float value = stat[idat];

        if (model.size() == stat.size())
          value -= model[idat];

        bool zap = false;

        if (cutoff_threshold_min > 0 &&
	    value < Q1 - cutoff_threshold_min * IQR)
          {
#ifdef _DEBUG
	    cerr << "TOO LOW isubint=" << isubint 
                 << " ichan=" << ichan << " ipol=" << ipol  << endl;
#endif
	    zap = true;
	    too_low ++;
          }
    
        if (cutoff_threshold_max > 0 &&
	    value > Q3 + cutoff_threshold_max * IQR)
          {
#ifdef _DEBUG
	    cerr << "TOO HIGH isubint=" << isubint 
                 << " ichan=" << ichan << " ipol=" << ipol  << endl;
#endif
	
            zap = true;
            too_high ++;
          }
    
        if (zap)
          mask[imask] = 0;
      }
    }
  }
#ifdef _DEBUG
  cerr << "too high=" << too_high << " too low=" << too_low << endl;
#endif

  return too_high + too_low;
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::InterQuartileRange::get_interface ()
{
  return new Interface (this);
}

Pulsar::InterQuartileRange::Interface::Interface (InterQuartileRange* instance)
{
  if (instance)
    set_instance (instance);

  add( &InterQuartileRange::get_cutoff_threshold_min,
       &InterQuartileRange::set_cutoff_threshold_min,
       "cutmin", "Outlier threshold: Q1-cutmin*IQR - Q3+cutmax*IQR" );

  add( &InterQuartileRange::get_cutoff_threshold_max,
       &InterQuartileRange::set_cutoff_threshold_max,
       "cutmax", "Outlier threshold: Q1-cutmin*IQR - Q3+cutmax*IQR" );
}

