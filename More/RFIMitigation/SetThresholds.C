/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SetThresholds.h"

#include <algorithm>
#include <numeric>
#include <utility>
#include <cassert>

// #define _DEBUG 1

using namespace std;

Pulsar::SetThresholds::SetThresholds ()
{
  cutoff_max_set = cutoff_min_set = false;
  cutoff_max = cutoff_min = 0.0;
}

unsigned Pulsar::SetThresholds::update_mask (std::vector<float> &mask, 
					     std::vector<float> &stat,
					     std::vector<float> &model,
					     unsigned nsubint,
					     unsigned nchan,
					     unsigned npol)
{
#if _DEBUG
  cerr << "SetThresholds::update_mask nsubint=" << nsubint
       << " nchan=" << nchan << " npol=" << npol 
       << " stat.sz=" << stat.size() << " model.sz=" << model.size()
       << " mask.sz=" << mask.size() << endl;
#endif

  unsigned too_high = 0;
  unsigned too_low = 0;

  for (unsigned ipol=0; ipol < npol; ipol++)
  { 
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

        if (cutoff_min_set && value < cutoff_min)
        {
#ifdef _DEBUG
	  cerr << "TOO LOW isub=" << isub 
	       << " ichan=" << ichan << " ipol=" << ipol  << endl;
#endif
	  zap = true;
	  too_low ++;
	}
    
        if (cutoff_max_set && value > cutoff_max)
        {
#ifdef _DEBUG
	  cerr << "TOO HIGH isub=" << isub 
	       << " ichan=" << ichan << " ipol=" << ipol
	       << " val=" << value << endl;
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
TextInterface::Parser* Pulsar::SetThresholds::get_interface ()
{
  return new Interface (this);
}

Pulsar::SetThresholds::Interface::Interface (SetThresholds* instance)
{
  if (instance)
    set_instance (instance);

  add( &SetThresholds::get_cutoff_min,
       &SetThresholds::set_cutoff_min,
       "cutmin", "Lower cutoff value" );

  add( &SetThresholds::get_cutoff_max,
       &SetThresholds::set_cutoff_max,
       "cutmax", "Upper cutoff value" );
}

