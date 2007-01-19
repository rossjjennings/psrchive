/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"
#include "Pulsar/Profile.h"
#include "Error.h"

using namespac std;

bool tscrunch_weighted_midtime = true;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Integration::tscrunch
//
/*!
  \param nscrunch number of neighbouring Integrations to add. 
                  If nscrunch == 0, then add all Integrations together
 */
void Pulsar::Archive::tscrunch (unsigned nscrunch)
{
  unsigned nsub = get_nsubint();
  
  if (nscrunch == 1 || nsub < 2)
    return;
  
  // if nscrunch == 0, default is to scrunch all sub_ints
  if (nscrunch == 0)
    nscrunch = nsub;

  // Account for custom Integration ordering:

  IntegrationOrder* order = get<IntegrationOrder>();
  if (order) {
    order->combine(this, nscrunch);
    return;
  }

  if (has_model() && verbose > 1)  {
    bool all_zero = true;
    for (unsigned isub=0; isub < nsub; isub++)
      if (!get_Integration(isub)->zero_phase_aligned)
        all_zero = false;

    if (!all_zero)
      cerr << "Pulsar::Archive::tscrunch WARNING"
        " not all Integrations aligned to polyco" << endl;
  }

  unsigned newsub = nsub / nscrunch;
  
  // if there will be subints left over, scrunch them up
  // and tack onto the end
  
  if (nsub % nscrunch)
    newsub += 1;
  
  if (verbose > 2) cerr << "Pulsar::Archive::tscrunch - scrunching " 
		    << nsub << " Integrations by " << nscrunch << endl;
  
  double dm = get_dispersion_measure();
  bool must_dedisperse = dm != 0 && !get_dedispersed();

  double rm = get_rotation_measure();
  bool must_defaraday = rm != 0 && !get_faraday_corrected() && get_npol() == 4;

  unsigned save_nscrunch = nscrunch;

  try {
    
    for (unsigned isub=0; isub < newsub; isub++) {
      
      if (verbose > 2) cerr << "Pulsar::Archive::tscrunch resulting subint " 
			<< isub+1 << "/" << newsub << endl;

      Integration* result = get_Integration (isub);

      unsigned start = isub * nscrunch;
      
      // the last Integration may have less than nscrunch contributions
      if (start+nscrunch >= nsub)
	nscrunch = nsub - start;

      // //////////////////////////////////////////////////////////////////////
      //
      //  compute the new duration and weighted mid-time of the result
      //
      // //////////////////////////////////////////////////////////////////////

      double duration = 0.0;
      double total_weight = 0.0;

      for (unsigned iadd=0; iadd < nscrunch; iadd++) {

        Integration* cur = get_Integration (start+iadd);

        duration += cur->get_duration();

        if (tscrunch_weighted_midtime)
          for (unsigned ichan=0; ichan < cur->get_nchan(); ichan++)
            total_weight += cur->get_weight (ichan);
        else
          total_weight += 1.0;
  
      }

      result->set_duration (duration);

      MJD epoch;

      for (unsigned iadd=0; iadd < nscrunch; iadd++) {

        Integration* cur = get_Integration (start+iadd);

        if (tscrunch_weighted_midtime) {
          double weight = 0;
          for (unsigned ichan=0; ichan < cur->get_nchan(); ichan++)
            weight += cur->get_weight (ichan);
          epoch += weight/total_weight * cur->get_epoch();
        }
        else
          epoch += 1.0/total_weight * cur->get_epoch();
  
      }

      // //////////////////////////////////////////////////////////////////////
      //
      // round epoch to nearest integer period
      //
      // //////////////////////////////////////////////////////////////////////
  
      if (get_type() == Signal::Pulsar) {
  
        // ensure that the polyco includes the new integration time
        update_model (epoch);
  
        if (model) {
  
          // get the time of the first subint to be integrated into isub
          MJD firstmjd = get_Integration (start) -> get_epoch ();
          // get the phase at the time of the first subint
          Phase first_phase = model->phase(firstmjd);
  
          // get the phase at the midtime of the result
          Phase mid_phase = model->phase (epoch);
          // get the period at the midtime of the result
          double period = model->period (epoch);
  
          // set the phase at the midtime equal to that of the first subint
          Phase desired (mid_phase.intturns(), first_phase.fracturns());
          epoch = model->iphase (desired);
  
          if (verbose > 2)
            cerr << "Archive::tscrunch result phase = "
                 << model->phase(epoch) << endl;

          result->set_folding_period (period);

        }

      }

      result->set_epoch (epoch);

      // //////////////////////////////////////////////////////////////////////
      //
      // integrate Profile data
      //
      // //////////////////////////////////////////////////////////////////////

      for (unsigned ichan=0; ichan < get_nchan(); ichan++) {
	
	if (verbose > 2) 
	  cerr << "Pulsar::Archive::tscrunch weighted_frequency chan="
	       << ichan << endl;
	
	double reference_frequency = 0.0;

	reference_frequency = weighted_frequency (ichan, start,start+nscrunch);
	
	if (verbose > 2) 
	  cerr << "Pulsar::Archive::tscrunch ichan=" << ichan
	       << " new frequency=" << reference_frequency << endl;

    
	for (unsigned iadd=0; iadd < nscrunch; iadd++) {

	  Integration* subint = get_Integration(start+iadd);

	  if (must_dedisperse)
	    subint->dedisperse (ichan, ichan+1, dm, reference_frequency);

	  if (must_defaraday)
	    subint->defaraday (ichan, ichan+1, rm, reference_frequency);

	  subint->set_centre_frequency (ichan, reference_frequency);

	}
	
	if (verbose > 2) 
	  cerr <<  "Pulsar::Archive::tscrunch sum profiles" << endl;
	
	for (unsigned ipol=0; ipol < get_npol(); ++ipol) {
	  
	  Profile* avg = get_Profile (isub, ipol, ichan);
	  Profile* add = get_Profile (start, ipol, ichan);
	  
	  *(avg) = *(add);
	  
	  for (unsigned jsub=1; jsub<nscrunch; jsub++) {
	    add = get_Profile (start+jsub, ipol, ichan);
	    *(avg) += *(add);
	  }
	  
	} // for each poln

      } // for each channel

      // //////////////////////////////////////////////////////////////////////
      //
      // integrate Extension data
      //
      // //////////////////////////////////////////////////////////////////////

      for (unsigned iadd=0; iadd < nscrunch; iadd++) {

        Integration* cur = get_Integration (start+iadd);

        if (iadd == 0)
          // transfer the Extensions from the start Integration to the result
          for (unsigned iext = 0; iext < cur->get_nextension(); iext++)
            result->add_extension( cur->get_extension(iext) );
        else
          // integrate the Extensions into the result
          for (unsigned iext = 0; iext < result->get_nextension(); iext++)
            result->get_extension(iext)->integrate (cur);

      }

      // //////////////////////////////////////////////////////////////////////
      //
      // update all Extensions
      //
      // //////////////////////////////////////////////////////////////////////

      for (unsigned iext = 0; iext < result->get_nextension(); iext++) {
        Integration::Extension* ext = result->get_extension(iext);
        ext->update (result);
      }

    } // for each integrated result

  } // end try block
  
  catch (Error& err) {
    throw err += "Pulsar::Archive::tscrunch";
  }

  resize (newsub);
}

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
					    unsigned start, unsigned end) 
  const
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

  try {
    for (unsigned isubint=start; isubint < end; isubint++) {
      
      const Profile* prof = get_Profile (isubint, ipol, ichan);
      
      double freq   = prof->get_centre_frequency();
      double weight = prof->get_weight();
      
      //if (verbose > 2)
      //cerr << "Pulsar::Archive::weighted_frequency [" << isubint << "]"
      //  " freq=" << freq << " wt=" << weight << endl;
      
      freqsum += freq * weight;
      weightsum += weight;
      
      if (isubint == start)
	fstart = freq;
      if (isubint == end-1)
	fend = freq;
    }
  }
  catch (Error& err) {
    throw err += "Pulsar::Archive::weighted_frequency";
  }
  
  double result = 0.0;
  
  if (weightsum != 0.0)
    result = freqsum / weightsum;
  else
    result = 0.5 * ( fstart + fend );
  
  // Nearest kHz
  result = 1e-3 * double( int(result*1e3) );
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
