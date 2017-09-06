/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FrequencyScale.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "pairutil.h"

#include <iostream>
using namespace std;

Pulsar::FrequencyScale::FrequencyScale ()
{
  reverse = false;
}

static void get_minmax_freq (const Pulsar::Archive* data, 
    double& min, double& max)
{
  double bw = data->get_bandwidth();
  if (data->get_nsubint())
  {
    Reference::To<const Pulsar::Integration> subint = data->get_Integration(0);
    min = subint->get_centre_frequency(0);
    max = subint->get_centre_frequency(0);
    for (unsigned i=1; i<data->get_nchan(); i++)
    {
      double chfreq = subint->get_centre_frequency(i);
      if (chfreq<min) { min=chfreq; }
      if (chfreq>max) { max=chfreq; }
    }
    if (bw<0.0) std::swap(min,max); 
  }
  else
  {
    double freq = data->get_centre_frequency();
    max = freq + bw/2.0;
    min = freq - bw/2.0;
  }
}

void Pulsar::FrequencyScale::init (const Archive* data)
{
  // PlotScale::num_indeces
  num_indeces = data->get_nchan();

  double min_freq, max_freq;
  get_minmax_freq(data, min_freq, max_freq);

  if (reverse)
    set_world (std::pair<float,float>(max_freq, min_freq));

  if (!get_minmaxset())
    set_minmax (min_freq, max_freq);
}

void Pulsar::FrequencyScale::get_indeces (const Archive* data, 
					  unsigned& min, unsigned& max) const
{
  PlotScale::get_indeces (data->get_nchan(), min, max);
}

std::string Pulsar::FrequencyScale::get_label () const
{
  return "Frequency (MHz)";
}

void Pulsar::FrequencyScale::get_ordinates (const Archive* data,
					    std::vector<float>& x_axis) const
{
  x_axis.resize (data->get_nchan());

  double min_freq, max_freq;
  get_minmax_freq(data, min_freq, max_freq);

  if (data->get_nsubint())
  {
    Reference::To<const Integration> subint = data->get_Integration(0);
    for (unsigned ibin = 0; ibin < x_axis.size(); ibin++) 
      x_axis[ibin] = subint->get_centre_frequency(ibin);

  }
  else
  {
    double bw = max_freq - min_freq;
    for (unsigned ibin = 0; ibin < x_axis.size(); ibin++)
      x_axis[ibin] = min_freq + bw * (double(ibin) + 0.5) / x_axis.size();
  }
}

Pulsar::FrequencyScale::Interface::Interface (FrequencyScale* instance)

{
  if (instance)
    set_instance (instance);

  import( PlotScale::Interface() );

  add( &PlotScale::get_index_range,
       &PlotScale::set_index_range,
       "chan", "Frequency channel index range" );

  add( &FrequencyScale::get_reverse,
       &FrequencyScale::set_reverse,
       "reverse", "Reverse order" );  
}
