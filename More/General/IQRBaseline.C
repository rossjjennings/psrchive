/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/IQRBaseline.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <iostream>
#include <algorithm>

using namespace std;

// #define _DEBUG 1

Pulsar::IQRBaseline::IQRBaseline ()
{
  set_threshold (1.5);
}

Pulsar::IQRBaseline* Pulsar::IQRBaseline::clone () const
{
  return new IQRBaseline (*this);
}

void Pulsar::IQRBaseline::get_bounds (PhaseWeight* weight, float& lower, float& upper)
{
  unsigned nbin = profile->get_nbin();

  vector<float> vals (nbin);
  unsigned mbin = 0;

  for (unsigned ibin=0; ibin < nbin; ibin++)
    if ((*weight)[ibin] != 0.0)
    {
      vals[mbin] = profile->get_amps()[ibin];
      mbin ++;
    }

  std::sort( vals.begin(), vals.begin() + mbin );

  unsigned Q1 = mbin / 4;
  unsigned Q3 = (3 * mbin) / 4;

  float IQR = vals[Q3] - vals[Q1];
  lower = vals[Q1] - threshold * IQR;
  upper = vals[Q3] + threshold * IQR;
}

class Pulsar::IQRBaseline::Interface 
  : public TextInterface::To<IQRBaseline>
{
public:
  Interface (IQRBaseline* instance)
  {
    if (instance)
      set_instance (instance);

    add( &IterativeBaseline::get_threshold,
	 &IterativeBaseline::set_threshold,
	 "threshold", "cutoff threshold used to avoid outliers" );
  }

  std::string get_interface_name () const { return "iqr"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::IQRBaseline::get_interface ()
{
  return new Interface (this);
}

