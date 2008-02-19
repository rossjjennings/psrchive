/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/TimeIntegrate.h"

using namespace std;

static Pulsar::TimeIntegrate* operation = 0;
static Pulsar::TimeIntegrate::EvenlySpaced* policy = 0;

static void static_init ()
{
  operation = new Pulsar::TimeIntegrate;
  policy    = new Pulsar::TimeIntegrate::EvenlySpaced;

  operation->set_range_policy( policy );
}

/*!
  \param nscrunch number of neighbouring Integrations to add. 
                  If nscrunch == 0, then add all Integrations together
 */
void Pulsar::Archive::tscrunch (unsigned nscrunch)
{
  if (!policy)
    static_init ();

  policy->set_nintegrate (nscrunch);
  operation->transform (this);
}

#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::weighted_frequency
//
/*!
  \return the new weighted centre frequency (in MHz, to nearest kHz)
  \param  ichan the index of the requested frequency channel
  \param  start the index of the first Integration to include in the mean
  \param  end one more than the index of the last Integration
*/
double Pulsar::Archive::weighted_frequency (unsigned ichan,
					    unsigned start,
					    unsigned end) const
{
  unsigned nsubint = get_nsubint();

  if (end == 0)
    end = nsubint;

  // for now, ignore poln
  unsigned ipol = 0;

  if (nsubint == 0)
    throw Error (InvalidRange, "Pulsar::Archive::weighted_frequency",
                 "nsubint == 0");

  if (start >= nsubint)
    throw Error (InvalidRange, "Pulsar::Archive::weighted_frequency",
		 "start=%d nsubint=%d", start, nsubint);

  if (end > nsubint)
    throw Error (InvalidRange, "Pulsar::Archive::weighted_frequency",
		 "end=%d nsubint=%d", end, nsubint);

  double weightsum = 0.0;
  double freqsum = 0.0;

  double fstart = 0.0;
  double fend = 0.0;

  for (unsigned isubint=start; isubint < end; isubint++) try {
      
    const Profile* prof = get_Profile (isubint, ipol, ichan);
    
    double freq   = prof->get_centre_frequency();
    double weight = prof->get_weight();
    
    if (verbose > 2)
      cerr << "Pulsar::Archive::weighted_frequency [" << isubint << "]"
	" freq=" << freq << " wt=" << weight << endl;
      
    freqsum += freq * weight;
    weightsum += weight;
    
    if (isubint == start)
      fstart = freq;
    if (isubint == end-1)
      fend = freq;
  }
  catch (Error& err)
  {
    throw err += "Pulsar::Archive::weighted_frequency";
  }
  
  double result = 0.0;
  
  if (weightsum != 0.0)
  {
    result = freqsum / weightsum;
    if (verbose)
      cerr << "Pulsar::Archive::weighted_frequency mean=" << result << endl;
  }
  else
  {
    result = 0.5 * ( fstart + fend );
    if (verbose)
      cerr << "Pulsar::Archive::weighted_frequency mid=" << result << endl;
  }

  // Nearest kHz
  result = 1e-3 * double( int(result*1e3) );

  if (verbose)
    cerr << "Pulsar::Archive::weighted_frequency kHz=" << result*1e3 << endl;

  return result;
}

/////////////////////////////////////////////////////////////////////////////
//
/*!
  Useful wrapper for Archive::tscrunch
*/
void Pulsar::Archive::tscrunch_to_nsub (unsigned new_nsub)
{
  if (new_nsub <= 0)
    throw Error (InvalidParam, "Pulsar::Archive::tscrunch_to_nsub",
		 "Invalid nsub request (new_nsub=%d nsub=%d)",
		 new_nsub,get_nsubint());
  else if (get_nsubint() < new_nsub)
    throw Error (InvalidParam, "Pulsar::Archive::tscrunch_to_nsub",
		 "Archive has too few subints (new_nsub=%d nsub=%d)",
		 new_nsub,get_nsubint());
  else
    tscrunch(get_nsubint() / new_nsub);
}
