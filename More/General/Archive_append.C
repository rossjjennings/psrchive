/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>

#include "Pulsar/Config.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"

#include "Pulsar/Parameters.h"
#include "Pulsar/Predictor.h"

#include "Error.h"

using namespace std;

bool Pulsar::Archive::append_chronological 
= Pulsar::config.get<bool>("append_chronological", false);

bool Pulsar::Archive::append_must_match
= Pulsar::config.get<bool>("append_must_match", true);

/*!
  Maximum in seconds
*/
double Pulsar::Archive::append_max_overlap
= Pulsar::config.get<double>("append_max_overlap", 30.0);


/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Archive::append 
//
/*! 
  Add clones of the Integrations in arch to this.
 */
void Pulsar::Archive::append (const Archive* arch)
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::append current nsub=" << get_nsubint()
	 << " adding nsub=" << arch->get_nsubint() << endl;

  if (arch->get_nsubint() == 0)
    return;
  
  if (get_nsubint() == 0) {
    copy (arch);
    return;
  }

  if (arch == this)
    throw Error (InvalidParam, "Pulsar::Archive::append",
		 "cannot append archive to self");

  // Account for custom Integration ordering:
  
  IntegrationOrder* order_this = get<IntegrationOrder>();
  const IntegrationOrder* order_that = arch->get<IntegrationOrder>();
  
  if (order_this && order_that) {
    if (order_this->get_IndexState() == order_that->get_IndexState()) {
      order_this->append(this, arch);
      return;
    }
    else
      throw Error(InvalidState, "Archive::append",
		  "The Archives have incompatible IntegrationOrder extensions");
  }
  else if (!order_this && order_that) {
    throw Error(InvalidState, "Archive::append",
		"The Archives have incompatible IntegrationOrder extensions");
  }
  else if (order_this && !order_that) {
    Reference::To<Pulsar::Archive> copy = arch->clone();
    copy->add_extension(order_this->clone());
    // This next line is a bit tricky... There are issues when the index
    // you are using is cyclical and you have more than one wrap across
    // which to define the subint resolution. In this case, the second
    // argument to the organise function needs more thought.
    // AWH 30/12/2003
    copy->get<IntegrationOrder>()->organise(copy, copy->get_nsubint());
    order_this->append(this, copy);
    return;
  }

  if (verbose == 3)
    cerr << "Pulsar::Archive::append compare" << endl;
  
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
    
    if (verbose == 3) 
      cerr << "Pulsar::Archive::append ensuring chronological order" << endl;
    
    MJD curlast  = end_time ();
    MJD newfirst = arch->start_time();

    if (curlast > newfirst + append_max_overlap)
      throw Error (InvalidState, "Archive::append",
		   "startime overlaps or precedes endtime");

  }

  /* if the Integrations are already properly phased to the polycos,
     then no corrections are needed */
  bool phase_aligned = zero_phase_aligned () && arch->zero_phase_aligned ();

  unsigned old_nsubint = get_nsubint();

  if (verbose == 3)
    cerr << "Pulsar::Archive::append call IntegrationManager::append" << endl;
 
  IntegrationManager::append (arch);
  
  // if observation is not a pulsar, no further checks required
  if (get_type() != Signal::Pulsar) {
    if (verbose == 3)
      cerr << "Pulsar::Archive::append no pulsar; no predictor to correct"
	   << endl;
    return;
  }
  
  // if neither archive has a polyco, no correction needed
  if (!model && !arch->model) {
    if (verbose == 3)    
      cerr << "Pulsar::Archive::append no predictor to correct" << endl;
    return;
  }

  bool equal_ephemerides = ephemeris && arch->ephemeris 
    && ephemeris->equals (arch->ephemeris);

  /*
    If all of the old and new integrations have been zero phase
    aligned according to the phase predictor models of both archives,
    and the parameter files from which those predictors were generated
    are equal, then keep this information and do not generate any new
    predictors.
  */

  if (verbose == 3) {
    if (!phase_aligned)
      cerr << "Pulsar::Archive::append "
	"zero phase aligned flags not set" << endl;
    if (!equal_ephemerides)
      cerr << "Pulsar::Archive::append "
	"archives have different ephemerides" << endl;
  }

  if (phase_aligned && equal_ephemerides) {

    if (verbose == 3)
      cerr << "Pulsar::Archive::append "
	"zero phase aligned and equal ephemerides" << endl;
 
    model->insert (arch->model);

    for (unsigned isub=0; isub < get_nsubint(); isub++)
      get_Integration(isub)->zero_phase_aligned = true;

    return;
  }

  if (model && !ephemeris)
    return;

  if (verbose == 3)
    cerr << "Pulsar::Archive::append update predictor" << endl;

  update_model (old_nsubint);
  
  if (verbose == 3)
    cerr << "Pulsar::Archive::append phasing new integrations" << endl;

  // correct the new subints against their old model
  for (unsigned isub=old_nsubint; isub < get_nsubint(); isub++)
    apply_model (get_Integration(isub), arch->model);

  if (verbose == 3)
    cerr << "Pulsar::Archive::append exit" << endl;
}
