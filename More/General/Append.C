/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Append.h"

#include "Pulsar/ArchiveExpert.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/IntegrationOrder.h"

#include "Pulsar/Parameters.h"
#include "Pulsar/Predictor.h"
#include "Pulsar/Config.h"

#include "Error.h"

#include <iostream>
using namespace std;

Pulsar::Append::Append ()
{
  must_match = Pulsar::config.get<bool>("append_must_match", true);

  match.set_check_mixable ();
}

bool Pulsar::Append::stop (Archive* into, const Archive* from)
{
  if (into == from)
    throw Error (InvalidParam, "Pulsar::Append::append",
		 "cannot append archive to self");

  if (from->get_nsubint() == 0)
    return true;
  
  if (into->get_nsubint() == 0) {
    into->copy (from);
    return true;
  }

  return false;
}

void Pulsar::Append::check (Archive* into, const Archive* from)
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::Append::append compare" << endl;

  string reason;

  if (must_match && !match.match (into, from))
    throw Error (InvalidState, "Append::append", match.get_reason ());

  else if (!into->standard_match (from, reason))
    throw Error (InvalidState, "Append::append", reason);
}

/*! 
  Add clones of the Integrations in from to into
 */
void Pulsar::Append::append (Archive* into, const Archive* from)
{
  if (Archive::verbose == 3)
    cerr << "Pulsar::Append::append into nsub=" << into->get_nsubint()
	 << " from nsub=" << from->get_nsubint() << endl;

  if ( stop (into, from) )
    return;
  
  check (into, from);

  /* if the Integrations are already properly phased to the polycos,
     then no corrections are needed */
  bool aligned = into->expert()->zero_phase_aligned()
    && from->expert()->zero_phase_aligned ();

  unsigned into_nsubint = into->get_nsubint();

  Reference::To<Archive> clone = from->clone ();

  if (Archive::verbose == 3)
    cerr << "Pulsar::Append::append call combine" << endl;
 
  combine (into, clone);
  
  // if observation is not a pulsar, no further checks required
  if (into->get_type() != Signal::Pulsar) {
    if (Archive::verbose == 3)
      cerr << "Pulsar::Append::append no pulsar; no predictor to correct"
	   << endl;
    return;
  }
  
  // if neither archive has a polyco, no correction needed
  if (!into->has_model() && !from->has_model()) {
    if (Archive::verbose == 3)    
      cerr << "Pulsar::Append::append no predictor to correct" << endl;
    return;
  }

  bool equal_ephemerides = into->has_ephemeris() && from->has_ephemeris()
    && into->get_ephemeris()->equals (from->get_ephemeris());

  bool equal_models = into->has_model() && from->has_model()
    && into->get_model()->matches (from->get_model());

  /*
    If all of the old and new integrations have been zero phase
    aligned according to the phase predictor models of both archives,
    and the parameter files from which those predictors were generated
    are equal, then keep this information and do not generate any new
    predictors.
  */

  if (Archive::verbose == 3) {
    if (!aligned)
      cerr << "Pulsar::Append::append "
	"zero phase aligned flags not set" << endl;
    if (!equal_ephemerides)
      cerr << "Pulsar::Append::append "
	"archives have different ephemerides" << endl;
    if (!equal_models)
      cerr << "Pulsar::Append::append "
	"archives have different phase models" << endl;
  }

  if (aligned && equal_ephemerides && equal_models) {

    if (Archive::verbose == 3)
      cerr << "Pulsar::Append::append "
	"zero phase aligned and equal ephemerides" << endl;
 
    into->expert()->get_model()->insert (from->get_model());

    for (unsigned isub=0; isub < into->get_nsubint(); isub++)
      into->get_Integration(isub)->expert()->set_zero_phase_aligned (true);

    return;
  }

  if (into->has_model() && !into->has_ephemeris())
    return;

  if (Archive::verbose == 3)
    cerr << "Pulsar::Append::append update predictor" << endl;

  into->expert()->update_model (into_nsubint);
  
  if (Archive::verbose == 3)
    cerr << "Pulsar::Append::append phasing new integrations" << endl;

  /* Correct the new subints:

    At this point, 'into' includes pointers to the data in 'clone'.
    By correcting the data in 'clone', 'into' is also corrected.  */

  for (unsigned isub=0; isub < clone->get_nsubint(); isub++)
    into->expert()->apply_model (clone->get_Integration(isub),
				 from->get_model());

  if (Archive::verbose == 3)
    cerr << "Pulsar::Append::append exit" << endl;
}
