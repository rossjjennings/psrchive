/***************************************************************************
 *
 *   Copyright (C) 2009 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/


#include "Pulsar/DynamicBaselineSpectrumPlot.h"
#include "Pulsar/DynamicSpectrumPlot.h"
#include <Pulsar/Archive.h>
#include <Pulsar/Profile.h>
#include <Pulsar/PhaseWeight.h>
// Added
#include "Pulsar/Integration.h"

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

// Calculate values to fill in to plot array
void Pulsar::DynamicBaselineSpectrumPlot::get_plot_array( const Archive *data, 
    float *array )
{

  // Make a copy to dedisperse, need to do this for the 
  // PhaseWeight stuff to work correctly.
  Reference::To<Archive> data_copy;
  if (data->get_dedispersed()) 
    data_copy = const_cast<Archive *>(data);
  else {
    data_copy = data->clone();
    data_copy->dedisperse();
  }

  if (method==false) {
    printf(">> Using baseline method\n");

    // Only recalc baseline if needed 
    if (base==NULL || !reuse_baseline) { 
      base = data_copy->total()->get_Profile(0,0,0)->baseline(); 
    }
    
    int nsub = srange.second - srange.first + 1;
    int nchan = crange.second - crange.first + 1;
    int ii = 0;
    
    for( int ichan = crange.first; ichan <= crange.second; ichan++) 
      {
	for( int isub = srange.first; isub <= srange.second; isub++ )
	  {
	    Reference::To<const Profile> prof = 
	      data_copy->get_Profile(isub, pol, ichan);
	    
	    base->set_Profile(prof);
	    float value = 0.0;
	    
	    if (prof->get_weight()!=0.0) {
	      if (use_variance) {
		value = base->get_variance().get_value();
		value = sqrt(value);
	      } else {
		value = base->get_mean().get_value();
	      }
	    }
	    
	    array[ii] = value;
	    ii++;
	  }
      }
  } else {
    printf(">> Using full profile method\n");
    const unsigned nsub = data_copy->get_nsubint();
    const unsigned nchan = data_copy->get_nchan();
    const unsigned npol = data_copy->get_npol();
    int ii=0;
    
    for( int ichan = crange.first; ichan <= crange.second; ichan++) {
      for( int isub = srange.first; isub <= srange.second; isub++ ) {
	
	Integration* subint = data_copy->get_Integration (isub);
	Reference::To<Profile> profile = subint->get_Profile (pol,ichan);
	
	const unsigned nbin = profile->get_nbin();
	float* data = profile->get_amps();
	float value=0.0;
	
	if (profile->get_weight()!=0.0) {
	  float s1=0.0;
	  float s2=0.0;
	  // Compute variance and mean
	  for (unsigned ibin=0;ibin<nbin;ibin++) {
	    s1+=data[ibin];
	    s2+=data[ibin]*data[ibin];
	  } 
	  float mean=s1/(float) nbin;
	  float var=(s2-s1*mean)/((float) nbin-1);
	  if (use_variance)
	    value=var;
	  else
	    value=mean;
	}
	array[ii] = value;
	ii++;
      }
    }
  }
}
