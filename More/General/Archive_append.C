#include <iostream>

#include "Archive.h"
#include "Integration.h"
#include "Error.h"

#include "string_utils.h"  // for stringprintf

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
  if (get_observation_type() != Signal::Pulsar) {
    // may need to check cal phase, if available
    return;
  }

  // if the polycos are equivalent, no corrections needed
  if (model == arch->model)
    return;

  // if the current model does not span all Integrations, update the model
  if (!good_model (model))
    update_model (old_nsubint);

  // correct the new subints against their old model
  for (unsigned isub=old_nsubint; isub < subints.size(); isub++)
    apply_model (arch->model, subints[isub]);

}

void Pulsar::Archive::append (const vector<Integration*>& more_subints)
{
  for (unsigned isub=0; isub<more_subints.size(); isub++)
    subints.push_back ( new_Integration(more_subints[isub]) );

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
  if (get_state() != arch->get_state()) {
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
  if (get_basis() != arch->get_basis()) {
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
