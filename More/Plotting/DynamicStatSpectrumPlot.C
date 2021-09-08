/***************************************************************************
 *
 *   Copyright (C) 2021 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DynamicStatSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include <iostream>
using namespace std;
using namespace Pulsar;

Pulsar::DynamicStatSpectrumPlot::DynamicStatSpectrumPlot()
{
  statistic = NULL;
}

Pulsar::DynamicStatSpectrumPlot::~DynamicStatSpectrumPlot()
{
}

TextInterface::Parser* Pulsar::DynamicStatSpectrumPlot::get_interface ()
{
  return new Interface (this);
}

// Calculate values to fill in to plot array
void Pulsar::DynamicStatSpectrumPlot::get_plot_array ( const Archive *data, 
						       float *array )
{
  if (!statistic)
    throw Error (InvalidState, "DynamicStatSpectrumPlot::get_plot_array",
		 "Archive statistic not selected");
  
  std::pair<unsigned,unsigned> srange = get_subint_range (data);
  std::pair<unsigned,unsigned> crange = get_chan_range (data);

  int ii = 0;

  statistic->set_Archive (data);
  statistic->set_pol (ipol);

  for (int ichan = crange.first; ichan < crange.second; ichan++) 
  {
    for (int isub = srange.first; isub < srange.second; isub++ )
    {
      const Integration* subint = data->get_Integration (isub);

      float value = 0.0;

      if (subint->get_weight(ichan)!=0.0)
      {
	statistic->set_subint (isub);
	statistic->set_chan (ichan);
	value = statistic->get();
      }

      array[ii] = value;
      ii++;
    } // for each subint
  } // for each channel
}

//! Set the profile statistic
void DynamicStatSpectrumPlot::set_statistic (ArchiveStatistic* stat)
{
  statistic = stat;
}


//! Get the profile statistic
Pulsar::ArchiveStatistic* DynamicStatSpectrumPlot::get_statistic () const
{
  return statistic;
}
