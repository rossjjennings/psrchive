#include "Pulsar/FrequencyIntegrate.h"
#include "Pulsar/FrequencyIntegrate_EvenlySpaced.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Error.h"

void Pulsar::FrequencyIntegrate::transform (Integration* integration)
{
  if (Integration::verbose)
    cerr << "Pulsar::FrequencyIntegrate::transform"
      " nscrunch=" << nscrunch << " nchan=" << nchan 
	 << " dm=" << integration->get_dispersion_measure() << endl;

  unsigned subint_nchan = integration->get_nchan();
  unsigned subint_npol  = integration->get_npol();

  if (nscrunch == 1 || subint_nchan == 1 || nchan >= subint_nchan)
    return;    // nothing to scrunch

  range_policy->initialize (this, integration);
  unsigned output_nchan = range_policy->get_nrange();

  double dm = integration->get_dispersion_measure();
  bool must_dedisperse = dm != 0 && !integration->get_dedispersed();

  double rm = integration->get_rotation_measure();
  bool must_defaraday = subint_npol == 4 &&
    rm != 0 && !integration->get_faraday_corrected();

  unsigned start = 0;
  unsigned stop = 0;

  bool was_range_checking = range_checking_enabled;
  range_checking_enabled = false;

  for (unsigned ichan=0; ichan < output_nchan; ichan++) try {
      
    range_policy->get_range (ichan, start, stop);
    
    if (Integration::verbose)
      cerr << "Pulsar::FrequencyIntegrate::transform ichan=" << ichan << endl;
    
    double reference_frequency = integration->weighted_frequency (start,stop);

    if (must_dedisperse)
      integration->dedisperse (start, stop, dm, reference_frequency);

    if (must_defaraday)
      integration->defaraday (start, stop, rm, reference_frequency);
    
    for (unsigned ipol=0; ipol < subint_npol; ipol++)  {
      
      if (Integration::verbose)
	cerr << "Pulsar::FrequencyIntegrate::transform ipol=" << ipol << endl;

      Profile* output = integration->get_Profile (ipol, ichan);

      for (unsigned jchan=start; jchan<stop; jchan++) {
	Profile* input  = integration->get_Profile (ipol, jchan);
	if (jchan==start)
	  *(output) = *(input);
	else
	  *(output) += *(input);
      }
      
    }

    integration->set_centre_frequency (ichan, reference_frequency);

  }
  catch (Error& error) {
   range_checking_enabled = was_range_checking;
   throw error += "FrequencyIntegrate::transform";
  }

  range_checking_enabled = was_range_checking;

  if (Integration::verbose)
    cerr << "Pulsar::FrequencyIntegrate::transform resize" << endl;

  integration->resize (0, output_nchan, 0);

  if (Integration::verbose) 
    cerr << "Pulsar::FrequencyIntegrate::transform finish" << endl;
} 

//! Default constructor
Pulsar::FrequencyIntegrate::FrequencyIntegrate ()
{
  nchan = 0;
  nscrunch = 0;
  range_policy = new EvenlySpaced;
}

//! Set the number of output frequency channels
void Pulsar::FrequencyIntegrate::set_nchan (unsigned _nchan)
{
  nchan = _nchan;
  nscrunch = 0;
}

//! Get the number of output frequency channels
unsigned Pulsar::FrequencyIntegrate::get_nchan () const
{
  return nchan;
}

//! Set the number of channels to integrate
void Pulsar::FrequencyIntegrate::set_nscrunch (unsigned _nscrunch)
{
  nscrunch = _nscrunch;
  nchan = 0;
}

//! Get the number of channels to integrate
unsigned Pulsar::FrequencyIntegrate::get_nscrunch () const
{
  return nscrunch;
}

void Pulsar::FrequencyIntegrate::divide (unsigned div_nchan,
					 unsigned& nrange,
					 unsigned& spacing) const
{
  if (nchan) {
    // the number of output frequency channel ranges was specified
    nrange = nchan;
    // calculate the spacing
    spacing = div_nchan / nrange;
    if (div_nchan % nrange)
      spacing ++;
  }
  
  if (nscrunch) {
    // the spacing of frequency channels (scrunch) was specified
    spacing = nscrunch;
    // calculate the number of output frequency channel ranges
    nrange = div_nchan / spacing;
    if (div_nchan % spacing)
      nrange ++;
  }
}

void Pulsar::FrequencyIntegrate::set_range_policy (RangePolicy* policy)
{
  range_policy = policy;
}
