#include <iostream>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

#include "string_utils.h"  // for stringprintf

bool Pulsar::Archive::append_chronological = false;

bool Pulsar::Archive::append_must_match = true;

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

  if (append_must_match) {
    string reason;
    if (!mixable (arch, reason))
      throw Error (InvalidState, "Archive::append", reason);
  }

  if (append_chronological && get_nsubint() > 0) {

    if (verbose) 
      cerr << "Pulsar::Archive::append ensuring chronological order" << endl;

    MJD curlast  = end_time ();
    MJD newfirst = arch->start_time();

    if (curlast > newfirst + append_max_overlap)
      throw Error (InvalidState, "Archive::append", 
		   "startime overlaps or precedes endtime");

  }

  unsigned old_nsubint = get_nsubint();

  IntegrationManager::append (arch);

  // if observation is not a pulsar, no further checks required
  if (get_type() != Signal::Pulsar) {
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
  for (unsigned isub=old_nsubint; isub < get_nsubint(); isub++)
    apply_model (arch->model, get_Integration(isub));

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
bool Pulsar::Archive::mixable (const Archive* arch, string& reason) const
{
  if (!standard_match (arch, reason))
    return false;

  return match (arch, reason);
}

bool 
Pulsar::Archive::standard_match (const Archive* arch, string& reason) const
{
  if (get_state() != arch->get_state()) {
    reason = stringprintf ("polarimetric state mismatch: %s != %s",
			   Signal::state_string(get_state()),
			   Signal::state_string(arch->get_state()));
    return false;
  }
  
  if (get_type() != arch->get_type()) {
    reason = stringprintf ("observation type mismatch: %s != %s",
			   Signal::source_string(get_type()),
			   Signal::source_string(arch->get_type()));
    return false;
  }

  if (get_source() != arch->get_source()) {
    reason = "source name mismatch: "+get_source()+" != "+arch->get_source();
    return false;
  }

  double cf1 = get_centre_frequency();
  double cf2 = arch->get_centre_frequency();
  double dfreq = fabs (cf2 - cf1);

  if (dfreq > 0.2 * cf1) {
    reason = stringprintf ("centre frequency mismatch: %lf and %lf", cf1, cf2);
    return false;
  }

  if (get_nbin() != arch->get_nbin()) {
    reason = stringprintf ("numbers of bins mismatch: %d != %d",
			   get_nbin(), arch->get_nbin());
    return false;
  }

  return true;
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
bool Pulsar::Archive::match (const Archive* arch, string& reason) const
{
  if (get_basis() != arch->get_basis()) {
    reason = stringprintf ("feed type mismatch: %d != %d", 
			   get_basis(), arch->get_basis());
    return false;
  }
  if (get_nchan() != arch->get_nchan()) {
    reason = stringprintf ("numbers of channels mismatch: %d != %d",
			   get_nchan(), arch->get_nchan());
    return false;
  }

  double cf1 = get_centre_frequency();
  double cf2 = arch->get_centre_frequency();
  double dfreq = fabs (cf2 - cf1);

  if (dfreq > match_max_frequency_difference) {
    reason = stringprintf ("centre frequency mismatch: %lf and %lf", cf1, cf2);
    return false;
  }

  double bw1 = get_bandwidth();
  double bw2 = arch->get_bandwidth();

  if ( ( bw1 != bw2 ) && !( match_opposite_sideband && (bw1 != -bw2) ) ) {
    reason = stringprintf ("bandwidth mismatch: %lf and %lf", bw1, bw2);
    return false;
  }
  
  // none of the above restrictions apply
  return true;
}

