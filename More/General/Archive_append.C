#include <iostream>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

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

  string reason;
  if (append_must_match) {
    if (!mixable (arch, reason))
      throw Error (InvalidState, "Archive::append", reason);
  }
  else {
    if (!standard_match (arch, reason))
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

