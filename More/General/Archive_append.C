#include <iostream>

#include "tempo++.h"
#include "string_utils.h"

#include "Archive.h"
#include "Integration.h"
#include "Error.h"


bool Pulsar::Archive::append_chronological = false;

/*!
  Maximum in seconds
*/
double Pulsar::Archive::append_max_overlap = 30.0;

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::append 
//
/*! 
  Add clones of the Integrations in arch to this.
 */
void Pulsar::Archive::append (const Archive* arch)
{
  if (verbose)
    cerr << "Pulsar::Archive::append entered\n";

  if (arch->get_nsubint() == 0)
    return;

  string reason;
  if (!mixable (arch, reason))
    throw Error (InvalidState, "Archive::append", reason);

  if (append_chronological && subints.size() > 0) {

    if (verbose) 
      cerr << "Pulsar::Archive::append ensuring chronological order" << endl;

    MJD curlast  = end_time ();
    MJD newfirst = arch->start_time();

    if (curlast > newfirst + append_max_overlap)
      throw Error (InvalidState, "Archive::append", 
		   "startime overlaps or precedes endtime");

  }

  unsigned old_nsubint = subints.size();

  append (arch->subints);

  // if observation is not a pulsar, no further checks required
  if (get_observation_type() != Observation::Pulsar) {
    // may need to check cal phase, if available
    return;
  }

  // if the polycos are equivalent, no corrections needed
  if (model == arch->model)
    return;

  // otherwise, update the model
  update_model (old_nsubint);

  // correct the new subints against their old model
  for (unsigned isub=old_nsubint; isub < subints.size(); isub++)
    apply_model (arch->model, subints[isub]);

}

bool Pulsar::Archive::need_create_model () const
{
  for (unsigned isub=0; isub < subints.size(); isub++)
    if ( model.i_nearest (subints[isub]->get_mid_time()) == -1 )
      return true;

  return false;
}

/*!  
  The polyco needs only describe the phase and period of every
  Integration in the subints array.  When the Integrations are
  separated by a large amount of time, the creation of a new polyco to
  completely span this time results in a huge polyco.dat and a huge
  waste of time.

  Therefore, this method attempts to create the minimum set of polyco
  polynomials required to describe the data.  If a match is not found
  in the current model, a single polynomial is created and appended to
  the current model.

  \param clear_model delete the old model after getting its attributes
*/
void Pulsar::Archive::create_updated_model (bool clear_model)
{
  if (get_observation_type() != Observation::Pulsar)
    throw Error (InvalidState, "Archive::create_updated_model",
		 "not a pulsar observation");

  int  maxha   = 12;
  char nsite   = model.get_telescope ();
  int  ncoeff  = model.get_ncoeff ();
  double freq  = model.get_freq ();
  double nspan = model.get_nspan ();

  if (clear_model)
    model = polyco();

  for (unsigned isub = 0; isub < subints.size(); isub++) {

    MJD time = subints[isub]->get_mid_time();

    if ( model.i_nearest (time) == -1 ) {
      // no match, create a new polyco for the Integration
      polyco part = Tempo::get_polyco (ephemeris, time, time,
				       nspan, ncoeff, maxha, nsite, freq);
      model.append (part);
    }

  }
}

/*!

  This method economizes on the number of times that the polynomial is
  re-created and the Integration set is re-aligned to the model.
  By setting the run-time only flag, Archive::model_updated, the old
  Integration set is flagged as no longer in need of correction.

  \param old_nsubint the number of subints in the old Integration set
*/
void Pulsar::Archive::update_model (unsigned old_nsubint)
{
  bool modelok = ! need_create_model ();

  if (modelok)
    return;

  polyco oldmodel;

  if (!model_updated)
    // store the old model
    oldmodel = model;

  // if the model has not already been updated, create a completely new polyco
  create_updated_model (!model_updated);

  // if previously updated, no need to correct the old subints
  if (model_updated)
    return;

  // correct the old subints with the old model
  for (unsigned isub = 0; isub < old_nsubint; isub++)
    apply_model (oldmodel, subints[isub]);
  
  model_updated = true;
}

void Pulsar::Archive::apply_model (const polyco& old, Integration* subint)
{
  if ( model.get_telescope() != old.get_telescope() )
    throw Error (InvalidState, "Archive::apply_model", "mismatched telescope");

  try {

    if (verbose) 
      model.unload (stderr);

    // get the MJD of the rising edge of bin zero
    MJD subint_mjd = subint -> get_mid_time();

    // get the phase shift due to differing observing frequencies between
    // old and current polyco
    Phase freq_shift_phase = 
      model.phase (subint_mjd, old.get_freq()) - model.phase (subint_mjd);

    // get the phase of the rising edge of bin zero
    Phase phase = model.phase (subint_mjd);
    
    // the Integration is rotated by -phase to bring zero phase to bin zero
    Phase dphase = freq_shift_phase - phase;
    
    double period = model.period (subint_mjd);
    double shift_time = dphase.fracturns() * period;
    
    if (verbose)
      cerr << "Archive::apply_model"
	   << " old MJD " << subint_mjd
	   << " old polyco phase " << old.phase(subint_mjd)
	   << " new polyco phase " << phase << endl
	
	   << " old freq " << old.get_freq()
	   << " new freq " << model.get_freq()
	   << " freq phase shift " << freq_shift_phase << endl
	
	   << " time shift      " << shift_time/86400.0
	   << " days  " << shift_time << " seconds "
	   << " total phase shift " << dphase.fracturns() << endl; 
    
    subint -> set_folding_period (period);  
    subint -> rotate (shift_time);
    
    if (verbose) {
      subint_mjd = subint -> get_mid_time();
      cerr << "Archive::apply_model"
	   << " new MJD "   << subint_mjd
	   << " new phase " << model.phase(subint_mjd)
	   << endl;
    }
  }
  catch (Error& err) {
    throw err += "Archive::apply_model";
  }
}

void Pulsar::Archive::append (const vector<Integration*>& more_subints)
{
  for (unsigned isub=0; isub<more_subints.size(); isub++)
    subints.push_back (more_subints[isub]->clone());

  set_nsubint (int(subints.size()));
}  

// ///////////////////////////////////////////////////////////////////////
//
// Archive::mixable
//
/*!
  This function defines the minimum set of observing parameters that
  should be equal (within certain limits) before two archives may be
  combined using append.

  \param arch pointer to Archive to be compared with this
  \retval reason if match fails, describes why in English
  \return true if arch matches this 
*/
bool Pulsar::Archive::mixable (const Archive* arch, string& reason)
{
  if (get_poln_state() != arch->get_poln_state()) {
    reason = "Archives have different polarimetric states";
    return false;
  }
  if (get_observation_type() != arch->get_observation_type()) {
    reason = "Archives have different observation type";
    return false;
  }
  if (get_source() != arch->get_source()) {
    reason = "Archives have different source names";
    return false;
  }
  if (get_nbin() != arch->get_nbin()) {
    reason = stringprintf ("Archives have different numbers of bins (%d!=%d)",
			   get_nbin(), arch->get_nbin());
    return false;
  }

  // none of the above restrictions apply
  return match (arch, reason);
}

/*!
  Maximum in MHz.
*/
double Pulsar::Archive::match_max_frequency_difference = 0.1; // 100 kHz

bool Pulsar::Archive::match_opposite_sideband = false;

// ///////////////////////////////////////////////////////////////////////
//
// Archive::match
//
/*!
  This function defines the minimum set of observing parameters that
  should be equal (within certain limits) before two archives are
  considered to match.  The test is lest stringent than
  Archive::mixable and is appropriate for testing the match between
  calibrator and pulsar observations, for instance.

  \param arch pointer to Archive to be compared with this
  \retval reason if match fails, describes why in English
  \return true if arch matches this 
*/
bool Pulsar::Archive::match (const Archive* arch, string& reason)
{
  if (get_feed_type() != arch->get_feed_type()) {
    reason = "Archives have different feed types";
    return false;
  }
  if (get_nchan() != arch->get_nchan()) {
    reason = "Archives have different numbers of channels";
    return false;
  }

  double cf1 = get_centre_frequency();
  double cf2 = arch->get_centre_frequency();
  double dfreq = fabs (cf2 - cf1);

  if (dfreq > match_max_frequency_difference) {
    reason = stringprintf ("Archives have too different center frequencies:"
			   "%lf and %lf", cf1, cf2);
    return false;
  }

  double bw1 = get_bandwidth();
  double bw2 = arch->get_bandwidth();

  if ( ( bw1 != bw2 ) && !( match_opposite_sideband && (bw1 != -bw2) ) ) {
    reason = stringprintf ("Archives have different bandwidths:"
			   "%lf and %lf", bw1, bw2);
    return false;
  }
  
  // none of the above restrictions apply
  return true;
}
