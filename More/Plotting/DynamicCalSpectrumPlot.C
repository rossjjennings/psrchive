/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicCalSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Integration.h>
#include <Pulsar/Profile.h>

Pulsar::DynamicCalSpectrumPlot::DynamicCalSpectrumPlot()
{
}

Pulsar::DynamicCalSpectrumPlot::~DynamicCalSpectrumPlot()
{
}

TextInterface::Parser* Pulsar::DynamicCalSpectrumPlot::get_interface ()
{
  return new Interface (this);
}

// Calculate values to fill in to plot array
void Pulsar::DynamicCalSpectrumPlot::get_plot_array( const Archive *data, 
    float *array )
{
  std::pair<unsigned,unsigned> srange = get_subint_range (data);
  std::pair<unsigned,unsigned> crange = get_chan_range (data);

  int nsub = srange.second - srange.first;

  // Cal levels
  std::vector< std::vector< Estimate<double> > > hi;
  std::vector< std::vector< Estimate<double> > > lo;

  for (int isub=srange.first; isub<srange.second; isub++)
  {
    data->get_Integration(isub)->cal_levels(hi, lo);

    for (int ichan=crange.first; ichan<crange.second; ichan++)
    {
      double val = 0.0;
      unsigned pol = ipol.get_value();
	
      if (data->get_Profile(isub,pol,ichan)->get_weight()!=0.0)
      {
        hi[pol][ichan] -= lo[pol][ichan];
        val = hi[pol][ichan].get_value();
      }

      array[(ichan-crange.first) * nsub + (isub-srange.first)] = val;

    }

  }

}

