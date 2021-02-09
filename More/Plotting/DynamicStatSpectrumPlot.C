/***************************************************************************
 *
 *   Copyright (C) 2021 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/DynamicStatSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/ProfileStatistic.h"

#include <iostream>
using namespace std;

Pulsar::DynamicStatSpectrumPlot::DynamicStatSpectrumPlot()
{
  stat = NULL;
}

Pulsar::DynamicStatSpectrumPlot::~DynamicStatSpectrumPlot()
{
}

TextInterface::Parser* Pulsar::DynamicStatSpectrumPlot::get_interface ()
{
  return new Interface (this);
}

// Calculate values to fill in to plot array
void Pulsar::DynamicStatSpectrumPlot::get_plot_array( const Archive *data, 
    float *array )
{

  std::pair<unsigned,unsigned> srange = get_subint_range (data);
  std::pair<unsigned,unsigned> crange = get_chan_range (data);

  int ii = 0;

  for (int ichan = crange.first; ichan < crange.second; ichan++) 
  {
    for (int isub = srange.first; isub < srange.second; isub++ )
    {
      Reference::To<const Profile> prof = 
	data->get_Profile(isub, pol, ichan);
	    
      float value = 0.0;

      if (prof->get_weight()!=0.0)
      {
	if (stat)
	{
          value = stat->get(prof);
	  //cerr << ichan << " " << isub << " " << text << " " << value << endl;
	}
      }

      array[ii] = value;
      ii++;
    } // for each subint
  } // for each channel
}
