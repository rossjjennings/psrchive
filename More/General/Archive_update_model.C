/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"
#include "Predict.h"
#include "Predictor.h"

#ifdef HAVE_TEMPO2
#include "T2Predictor.h"
#endif

#include <iostream>
using namespace std;

// ///////////////////////////////////////////////////////////////////////
//
// Archive::update_model
//
/*!  
  This method should be called when the ephemeris attribute is
  modified.  It may also be called when the tempo support files
  (e.g. leap.sec, ut1.dat) change.
*/
void Pulsar::Archive::update_model() 
{
  if (verbose > 2)
    cerr << "Pulsar::Archive::update_model" << endl;

  runtime_model = false;
  update_model (get_nsubint());
}


// ///////////////////////////////////////////////////////////////////////
//
// Archive::update_model
//
/*!
  This method economizes on the number of times that the polyco is
  re-created and the Integrations are re-aligned to the model.

  By setting the Archive::runtime_model attribute, the polyco is
  flagged as created by the currently available version of tempo and
  its run-time configuration files.

  By setting the Integration::zero_phase_aligned attribute, each
  sub-integration is flagged as no longer in need of alignment.

  If Archive::runtime_model or Integration::zero_phase_aligned is
  false, then the Integration is re-aligned using the difference
  between the old and new models.  (see Archive::apply_model)

  \param nsubint the number of Integrations to correct
*/
void Pulsar::Archive::update_model (unsigned nsubint) try {

  if (verbose > 2)
    cerr << "Pulsar::Archive::update_model nsubint=" << nsubint << endl;

  Reference::To<Predictor> oldmodel;

  if( !runtime_model ){
    // store the old model
    oldmodel = model;
  }

  // if the model has not already been updated, create a completely new polyco
  create_updated_model (!runtime_model);

  if (verbose > 2)
    cerr << "Pulsar::Archive::update_model checking first " 
         << nsubint << " Integrations" << endl;

  // correct the old Integrations with the old model
  for (unsigned isub = 0; isub < nsubint; isub++){
    if (!get_Integration(isub)->zero_phase_aligned)  {
      if (verbose > 2)
        cerr << "Pulsar::Archive::update_model phasing isub=" << isub << endl;
      apply_model (get_Integration(isub), oldmodel.ptr());
    }
  }

  runtime_model = true;
}
catch (Error& error) {
  throw error += "Pulsar::Archive::update_model";
}

// ///////////////////////////////////////////////////////////////////////
//
// Archive::create_updated_model
//
/*!  The polyco need only describe the phase and period of every
  Integration.  When the Integrations are separated by a large amount
  of time, the creation of a new polyco to completely span this time
  results in a huge polyco.dat and a huge waste of time.

  Therefore, this method attempts to create the minimum set of polyco
  polynomials required to describe the data.  If a match is not found
  in the current model, a single polynomial is created and appended to
  the current model.

  \param clear_model delete the current model after copying its attributes
*/
void Pulsar::Archive::create_updated_model (bool clear_model)
{
  if (get_type() != Signal::Pulsar)
    throw Error (InvalidState, "Pulsar::Archive::create_updated_model",
		 "not a pulsar observation");

  if (clear_model)
    for (unsigned isub = 0; isub < get_nsubint(); isub++)
      get_Integration(isub)->zero_phase_aligned = false;

  for (unsigned isub = 0; isub < get_nsubint(); isub++) {

    MJD time = get_Integration(isub)->get_epoch();
    update_model (time, clear_model);

    // clear the model only on the first time around the loop
    clear_model = false;

  }
}

/* This method ensures the the specified time is described by the
   polyco.  If a match is not found in the current model, a single
   polynomial is created and appended to the current model.

   \param clear_model delete the current model after copying its attributes
 */
void Pulsar::Archive::update_model (const MJD& time, bool clear_model)
{
  if (get_type() != Signal::Pulsar)
    throw Error (InvalidState, "Pulsar::Archive::update_model",
		 "not a pulsar observation");

  if (verbose > 2) cerr << "Pulsar::Archive::update_model epoch=" << time 
			 << " clear=" << clear_model << endl;

  if (model && !clear_model) try {

    model->phase (time);

    if (verbose > 2)
      cerr << "Pulsar::Archive::update_model current model spans epoch"
	   << endl;

    return;

  }
  catch (...) {

    if (verbose > 2)
      cerr << "Pulsar::Archive::update_model current model doesn't span epoch"
	   << endl;

  }

#ifdef HAVE_TEMPO2
  Tempo2::Predictor* t2model = dynamic_cast<Tempo2::Predictor*> (model.ptr());
  if (t2model)
    throw Error (InvalidState, "Pulsar::Archive::update_model",
		 "TEMPO2 Predictors not yet supported");
#endif

  polyco* t1model = dynamic_cast<polyco*> (model.ptr());
  
  if (!ephemeris || !t1model)
    return;

  static Tempo::Predict predict;

  predict.set_frequency ( get_centre_frequency() );
  predict.set_parameters ( *ephemeris );
  predict.set_asite ( get_telescope_code() );
  predict.set_maxha ( 12 );

  if (t1model && t1model->pollys.size() > 0) {

    predict.set_nspan ( (int) t1model->get_nspan() );
    predict.set_ncoef ( t1model->get_ncoeff() );

  }
  else {

    model = t1model = new polyco;

    predict.set_nspan ( 960 );
    predict.set_ncoef ( 12 );

  }

  if (clear_model)
    model = t1model = new polyco;
  
  if ( t1model->i_nearest (time) == -1 ) {
    if (verbose > 2)
      cerr << "Pulsar::Archive::update_model generating new predictor for"
	" epoch=" << time << endl;
    // no match, create a new polyco for the specified time
    polyco part = predict.get_polyco (time, time);
    t1model->append (part);
  }
}



