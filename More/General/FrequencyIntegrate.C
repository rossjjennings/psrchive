/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/FrequencyIntegrate.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "ModifyRestore.h"
#include "Error.h"

void Pulsar::FrequencyIntegrate::transform (Integration* integration)
{
  unsigned subint_nchan = integration->get_nchan();
  unsigned subint_npol  = integration->get_npol();

  if (nscrunch == 1 || subint_nchan <= 1 || nchan >= subint_nchan) {
    if (Integration::verbose) 
      cerr << "Pulsar::FrequencyIntegrate::transform nothing to do" << endl;
   return;
  }

  range_policy->initialize (this, integration);
  unsigned output_nchan = range_policy->get_nrange();

  double dm = integration->get_dispersion_measure();
  bool must_dedisperse = dm != 0 && !integration->get_dedispersed();

  double rm = integration->get_rotation_measure();
  bool must_defaraday = subint_npol == 4 &&
    rm != 0 && !integration->get_faraday_corrected();

  if (Integration::verbose)
    cerr << "Pulsar::FrequencyIntegrate::transform"
      " nscrunch=" << nscrunch << " nchan=" << nchan 
	 << " dm=" << dm << " rm=" << rm << endl;

  unsigned start = 0;
  unsigned stop = 0;

  ModifyRestore<bool> mod (range_checking_enabled, false);

  for (unsigned ichan=0; ichan < output_nchan; ichan++) try {
      
    range_policy->get_range (ichan, start, stop);
    
    double reference_frequency = integration->weighted_frequency (start,stop);

    if (Integration::verbose)
      cerr << "Pulsar::FrequencyIntegrate::transform ichan=" << ichan 
	   << " freq=" << reference_frequency << endl;

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
   throw error += "FrequencyIntegrate::transform";
  }

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

void Pulsar::FrequencyIntegrate::set_range_policy (RangePolicy* policy)
{
  range_policy = policy;
}


void Pulsar::FrequencyIntegrate::divide (unsigned div_nchan,
					 unsigned& nrange,
					 unsigned& spacing) const
{
  nrange = 1;
  spacing = div_nchan;

  if (nchan) {

    if (Integration::verbose)
      cerr << "Pulsar::FrequencyIntegrate::divide nchan=" << nchan << endl;

    // the number of output frequency channel ranges was specified
    nrange = nchan;
    // calculate the spacing
    spacing = div_nchan / nrange;
    if (div_nchan % nrange)
      spacing ++;

  }
  else if (nscrunch) {

    if (Integration::verbose)
      cerr << "Pulsar::FrequencyIntegrate::divide nscrunch=" << nscrunch
	   << endl; 

    // the spacing of frequency channels (scrunch) was specified
    spacing = nscrunch;
    // calculate the number of output frequency channel ranges
    nrange = div_nchan / spacing;
    if (div_nchan % spacing)
      nrange ++;

  }

  if (Integration::verbose)
    cerr << "Pulsar::FrequencyIntegrate::divide into " << nrange 
	 << " ranges with " << spacing << " channels per range" << endl;

}


using namespace Pulsar;

/* ***********************************************************************

   Evenly Spaced Frequency Channels Policy

   ************************************************************************ */

void FrequencyIntegrate::EvenlySpaced::initialize (FrequencyIntegrate* freq,
						   Integration* integration)
{
  freq->divide (integration->get_nchan(), nrange, spacing);
  subint_nchan = integration->get_nchan();
}

void FrequencyIntegrate::EvenlySpaced::get_range (unsigned irange, 
						  unsigned& start, 
						  unsigned& stop)
{
  if (irange >= nrange)
    throw Error (InvalidParam, 
		 "Pulsar::FrequencyIntegrate::EvenlySpaced::get_range",
		 "irange=%u >= nrange=%u", irange, nrange);

  start = irange * spacing;
  stop = start + spacing;
  
  if (stop > subint_nchan)
    stop = subint_nchan;

  if (Integration::verbose)
    cerr << "Pulsar::FrequencyIntegrate::EvenlySpaced::get_range\n "
      " irange=" << irange << " spacing=" << spacing <<
      " subint_nchan=" << subint_nchan <<
      " start=" << start << " stop=" << stop << endl;

}

/* ***********************************************************************

   Evenly Distributed Frequency Channels Policy

   ************************************************************************ */

void 
FrequencyIntegrate::EvenlyDistributed::initialize (FrequencyIntegrate* freq,
						   Integration* integration)
{
  unsigned subint_nchan = integration->get_nchan();
  unsigned good_nchan = 0;

  // count the number of good channels
  for (unsigned ichan=0; ichan < subint_nchan; ichan++)
    if (integration->get_weight(ichan) != 0)
      good_nchan ++;

#ifdef _DEBUG
  cerr << "EvenlyDistributed::initialize good_nchan=" << good_nchan << endl;
#endif

  // divide them up
  unsigned nrange = 0;
  unsigned spacing = 0;
  freq->divide (good_nchan, nrange, spacing);
  stop_indeces.resize (nrange);

  // and count off 'spacing' good channels per range
  unsigned curchan = 0;
  for (unsigned irange=0; irange < nrange; irange++) {

    for (good_nchan=0; good_nchan<spacing && curchan<subint_nchan; curchan++)
      if (integration->get_weight(curchan) != 0)
	good_nchan ++;

#ifdef _DEBUG
    cerr << "  stop[" << irange << "] = " << curchan << endl;
#endif

    stop_indeces[irange] = curchan;

  }

}

void FrequencyIntegrate::EvenlyDistributed::get_range (unsigned irange, 
						       unsigned& start, 
						       unsigned& stop)
{
  if (irange >= stop_indeces.size())
    throw Error (InvalidParam, 
		 "Pulsar::FrequencyIntegrate::EvenlyDistributed::get_range",
		 "irange=%u >= nrange=%u", irange, stop_indeces.size());

  if (irange > 0)
    start = stop_indeces[irange-1];
  else
    start = 0;

  stop = stop_indeces[irange];
}

