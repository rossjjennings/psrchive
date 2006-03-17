/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <iostream>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"
#include "Predict.h"

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
  if (verbose == 3)
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
void Pulsar::Archive::update_model (unsigned nsubint)
try {

  if (verbose == 3)
    cerr << "Pulsar::Archive::update_model nsubint=" << nsubint << endl;

  Reference::To<polyco> oldmodel;

  if (!runtime_model)
    // store the old model
    oldmodel = model;

  // if the model has not already been updated, create a completely new polyco
  create_updated_model (!runtime_model);

  if (verbose == 3)
    cerr << "Pulsar::Archive::update_model checking first " 
         << nsubint << " Integrations" << endl;

  // correct the old Integrations with the old model
  for (unsigned isub = 0; isub < nsubint; isub++)
    if (!get_Integration(isub)->zero_phase_aligned)  {
      if (verbose == 3)
        cerr << "Pulsar::Archive::update_model phasing isub=" << isub << endl;
      apply_model (get_Integration(isub), oldmodel.ptr());
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

  if (verbose == 3) cerr << "Pulsar::Archive::update_model time=" << time 
			 << " clear=" << clear_model << endl;

  if (!ephemeris) {
    model = 0;
    return;
  }

  static Tempo::Predict predict;

  predict.set_frequency ( get_centre_frequency() );
  predict.set_parameters ( *ephemeris );
  predict.set_asite ( get_telescope_code() );
  predict.set_maxha ( 12 );

  if (model && model->pollys.size() > 0) {

    predict.set_nspan ( (int) model->get_nspan() );
    predict.set_ncoef ( model->get_ncoeff() );

  }
  else {

    model = new polyco;

    predict.set_nspan ( 960 );
    predict.set_ncoef ( 12 );

  }

  if (clear_model)
    model = new polyco;

  if ( model->i_nearest (time) == -1 ) {
    if (verbose > 2)
      cerr << "Pulsar::Archive::update_model no model for " << time << endl;
    // no match, create a new polyco for the specified time
    polyco part = predict.get_polyco (time, time);
    model->append (part);
  }
}


// ///////////////////////////////////////////////////////////////////////
//
// Archive::apply_model
//
/*!
  This method aligns the Integration to the current polyco, as stored
  in the model attribute.  The Integration is rotated by the difference
  between the phase predicted by the current model and that predicted by
  the old model.
  \param old the old polyco used to describe subint
  \param subint pointer to the Integration to be aligned to the current model
*/
void Pulsar::Archive::apply_model (Integration* subint, const polyco* old)
{
  if ( !model )
    throw Error (InvalidState, "Pulsar::Archive::apply_model", "no polyco");

  if ( old && model->get_telescope() != old->get_telescope() ) {
    cerr << "Pulsar::Archive::apply_model telescope mismatch" << endl;
    old = 0;
  }

  try {

    // get the MJD of the rising edge of bin zero
    MJD subint_mjd = subint -> get_epoch();

    // get the phase shift due to differing observing frequencies between
    // old and current polyco
    Phase freq_shift_phase = 0;

    if (old)
      model->phase (subint_mjd, old->get_freq()) - model->phase (subint_mjd);

    // get the phase of the rising edge of bin zero
    Phase phase = model->phase (subint_mjd);
    
    // the Integration is rotated by -phase to bring zero phase to bin zero
    Phase dphase = freq_shift_phase - phase;
    
    double period = model->period (subint_mjd);
    double shift_time = dphase.fracturns() * period;
    
    if (verbose == 3) {

      Phase old_phase = (old) ? old->phase(subint_mjd) : 0;

      cerr << "Pulsar::Archive::apply_model"
	   << "\n  old MJD " << subint_mjd;

      if (old)
	cerr << "\n  old polyco phase " << old_phase
	     << "\n  old freq " << old->get_freq();

      cerr << "\n  new polyco phase " << phase
	   << "\n  new freq " << model->get_freq()
	   << "\n  freq phase shift " << freq_shift_phase
	
	   << "\n  time shift " << shift_time/86400.0 << " days" 
           << "\n             " << shift_time << " seconds "
	   << "\n  total phase shift " << dphase << endl; 
    }

    subint -> set_folding_period (period);  
    subint -> rotate (shift_time);
    subint -> zero_phase_aligned = true;

    if (verbose == 3) {
      subint_mjd = subint -> get_epoch();
      phase = model->phase(subint_mjd);
      cerr << "Pulsar::Archive::apply_model"
	   << "\n  new MJD "   << subint_mjd
	   << "\n  new phase " << phase
	   << endl;
    }
  }
  catch (Error& err) {
    throw err += "Pulsar::Archive::apply_model";
  }
}

// ///////////////////////////////////////////////////////////////////////
//
// Archive::good_model
//
/*!
  This method tests if the given model has a polynomial that applies
  to each Integration, returning false if a match is not found.
  \param test_model the polyco to be tested
 */
bool Pulsar::Archive::good_model (const polyco& test_model) const
{
  if (verbose == 3)
    cerr << "Pulsar::Archive::good_model testing polyco on " << get_nsubint()
	 << " integrations" << endl;

  unsigned isub=0;
  for (isub=0; isub < get_nsubint(); isub++)
    try {
      if ( test_model.i_nearest (get_Integration(isub)->get_epoch()) == -1 ) {
	if (verbose == 3) cerr << "Pulsar::Archive::good_model"
                             " polyco::i_nearest returns none" << endl;
	break;
      }
    }
    catch (...) {
      if (verbose == 3) cerr << "Pulsar::Archive::good_model"
                           " polyco::i_nearest throws exception" << endl;
      break;
    }
  
  if (isub < get_nsubint()) {

    if (verbose == 3)
      cerr << "Pulsar::Archive::good_model polyco failed on integration "
	   << isub << endl;

    return false;

  }

  if (verbose == 3)
    cerr << "Pulsar::Archive::good_model polyco passes test" << endl;

  return true;
}
