/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/DynamicBaselineSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/ProfileStats.h"

#include <iostream>
using namespace std;

Pulsar::DynamicBaselineSpectrumPlot::DynamicBaselineSpectrumPlot()
{
  method = false;
  use_variance = false;
  reuse_baseline = false;
  base = NULL;
}

Pulsar::DynamicBaselineSpectrumPlot::~DynamicBaselineSpectrumPlot()
{
}

TextInterface::Parser* Pulsar::DynamicBaselineSpectrumPlot::get_interface ()
{
  return new Interface (this);
}

// defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

// Calculate values to fill in to plot array
void Pulsar::DynamicBaselineSpectrumPlot::get_plot_array( const Archive *data, 
    float *array )
{
  if (verbose)
    cerr << "Pulsar::DynamicBaselineSpectrumPlot::get_plot_array" << endl;

  // Make a copy to dedisperse, need to do this for the 
  // PhaseWeight stuff to work correctly.
  Reference::To<Archive> data_copy;
  if (data->get_dedispersed()) 
    data_copy = const_cast<Archive *>(data);
  else {
    data_copy = data->clone();
    data_copy->dedisperse();
  }

  std::pair<unsigned,unsigned> srange = get_subint_range (data);
  std::pair<unsigned,unsigned> crange = get_chan_range (data);

  int ii = 0;

  // WvS - set up to use the profile statistics interpreter if needed  
  Reference::To<ProfileStats> stats;
  Reference::To<TextInterface::Parser> parser;

  // by default, use a PhaseWeight to compute mean or variance
  Reference::To<PhaseWeight> window;

  if (!expression.empty())
  {
    if (verbose)
      cout << ">> Using user-specified expression" << endl;

    stats = new ProfileStats;
    parser = stats->get_interface ();
    parser->set_prefix_name(false);
  }
  if (method==false)
  {
    if (verbose)
      cout << ">> Using baseline method" << endl;

    // Only recalc baseline if needed 
    if (!base || !reuse_baseline)
      base = data_copy->total()->get_Profile(0,0,0)->baseline(); 

    window = base;
  }
  else
  {
    if (verbose)
      cout << ">> Using full profile method" << endl;

    window = new PhaseWeight (data->get_nbin());
    window -> set_all (1.0);
  }

  for (int ichan = crange.first; ichan < crange.second; ichan++) 
  {
    for (int isub = srange.first; isub < srange.second; isub++ )
    {
      Reference::To<const Profile> prof = 
	get_Profile(data_copy, isub, ipol, ichan);
	    
      float value = 0.0;

      if (prof->get_weight()!=0.0)
      {
	if (stats)
	{
	  stats->set_Profile (prof);
	  string text = process( parser, expression );
	  value = fromstring<float>( text );
	  //cerr << ichan << " " << isub << " " << text << " " << value << endl;
	}
	else
	{
	  window->set_Profile(prof);

	  if (use_variance)
	  {
	    value = window->get_variance().get_value();
	    value = sqrt(value);
	  }
	  else
	    value = window->get_mean().get_value();
	}
      }

      array[ii] = value;
      ii++;
    } // for each subint
  } // for each channel
}
