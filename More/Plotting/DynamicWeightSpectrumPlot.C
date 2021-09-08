/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/DynamicWeightSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>

Pulsar::DynamicWeightSpectrumPlot::DynamicWeightSpectrumPlot()
{
  zero_check = false;
}

Pulsar::DynamicWeightSpectrumPlot::~DynamicWeightSpectrumPlot()
{
}

TextInterface::Parser* Pulsar::DynamicWeightSpectrumPlot::get_interface ()
{
  return new Interface (this);
}

// Calculate SNR to fill in to plot array
void Pulsar::DynamicWeightSpectrumPlot::get_plot_array( const Archive *data, 
    float *array )
{
  std::pair<unsigned,unsigned> srange = get_subint_range (data);
  std::pair<unsigned,unsigned> crange = get_chan_range (data);

  int ii = 0;

  for( int ichan = crange.first; ichan < crange.second; ichan++) 
  {
    for( int isub = srange.first; isub < srange.second; isub++ )
    {
      Reference::To<const Profile> prof = 
        get_Profile ( data, isub, ipol, ichan);
      array[ii] = prof->get_weight();
      ii++;
    }
  }

}

