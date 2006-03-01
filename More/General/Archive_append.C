#include <iostream>

#include "Pulsar/Config.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/IntegrationOrder.h"
#include "Error.h"

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
    cerr << "Pulsar::Archive::append entered\n";

  if (arch->get_nsubint() == 0)
    return;
  
  if( get_nsubint() == 0 ){
    operator=( *arch );
    return;
  }
  
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
    cerr << "Pulsar::Archive::append compare\n";
  
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
  
  unsigned old_nsubint = get_nsubint();

  if (verbose == 3)
    cerr << "Pulsar::Archive::append call IntegrationManager::append" << endl;
 
  IntegrationManager::append (arch);
  
  // if observation is not a pulsar, no further checks required
  if (get_type() != Signal::Pulsar) {
    if (verbose == 3)
      cerr << "Pulsar::Archive::append no pulsar; no polyco to correct" << endl;
    return;
  }
  
  // if neither archive has a polyco, no correction needed
  if (!model && !arch->model) {
    if (verbose == 3)    
      cerr << "Pulsar::Archive::append no polyco to correct" << endl;
    return;
  }

  /* if the polycos are equivalent and the Integrations are already
     properly phased to the polycos, then no corrections are needed */
  if (model && arch->model && *model == *(arch->model)) {
    bool zero_aligned = true;
    for (unsigned isub=0; isub < old_nsubint; isub++)
      if (!get_Integration(isub)->zero_phase_aligned)
	zero_aligned = false;

    for (unsigned jsub=0; jsub < arch->get_nsubint(); jsub++)
      if (!arch->get_Integration(jsub)->zero_phase_aligned)
	zero_aligned = false;

    if (zero_aligned) {
      if (verbose == 3)
	cerr << "Pulsar::Archive::append identical polycos [optimized]" <<endl;

      for (unsigned isub=old_nsubint; isub < get_nsubint(); isub++)
	get_Integration(isub)->zero_phase_aligned = true;

      return;
    }
  }

  if (verbose == 3)
    cerr << "Pulsar::Archive::append update polyco" << endl;

  update_model (old_nsubint);
  
  if (verbose == 3)
    cerr << "Pulsar::Archive::append phasing new integrations" << endl;

  // correct the new subints against their old model
  for (unsigned isub=old_nsubint; isub < get_nsubint(); isub++)
    apply_model (get_Integration(isub), arch->model.ptr());

  if (verbose == 3)
    cerr << "Pulsar::Archive::append exit" << endl;
}
