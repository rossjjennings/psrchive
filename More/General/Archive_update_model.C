#include <iostream>

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Error.h"

#include "ephio.h"

#include "tempo++.h"

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
  model_updated = false;
  update_model (get_nsubint());
}


// ///////////////////////////////////////////////////////////////////////
//
// Archive::update_model
//
/*!
  This method economizes on the number of times that the polyco is
  re-created and the Integrations are re-aligned to the model.
  By setting the run-time attribute, model_updated, the
  Integrations are flagged as no longer in need of correction.

  If model_updated is false, then the first nsubint Integrations are
  re-aligned using the difference between the old and new models.
  (see Archive::apply_model)

  \param nsubint the number of Integrations to correct
*/
void Pulsar::Archive::update_model (unsigned nsubint)
{
  polyco oldmodel;

  if (!model_updated)
    // store the old model
    oldmodel = model;

  // if the model has not already been updated, create a completely new polyco
  create_updated_model (!model_updated);

  // if previously updated, no need to correct the old Integrations
  if (model_updated)
    return;

  // correct the old Integrations with the old model
  for (unsigned isub = 0; isub < nsubint; isub++)
    apply_model (oldmodel, get_Integration(isub));
  
  model_updated = true;
}

// ///////////////////////////////////////////////////////////////////////
//
// Archive::create_updated_model
//
/*!  The polyco needs only describe the phase and period of every
  Integration.  When the Integrations are separated by a large amount
  of time, the creation of a new polyco to completely span this time
  results in a huge polyco.dat and a huge waste of time.

  Therefore, this method attempts to create the minimum set of polyco
  polynomials required to describe the data.  If a match is not found
  in the current model, a single polynomial is created and appended to
  the current model.

  \param clear_model delete the current model after copying its attributes */
void Pulsar::Archive::create_updated_model (bool clear_model)
{
  if (get_type() != Signal::Pulsar)
    throw Error (InvalidState, "Archive::create_updated_model",
		 "not a pulsar observation");

  for (unsigned isub = 0; isub < get_nsubint(); isub++) {

    MJD time = get_Integration(isub)->get_epoch();
    update_model (time, clear_model);

    // only clear the model on the first loop
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
    throw Error (InvalidState, "Archive::create_updated_model",
		 "not a pulsar observation");
  
  int  maxha;
  char nsite;
  int  ncoeff;
  double freq;
  double nspan;
  
  if (model.pollys.size() > 0) {
    maxha   = 12;
    nsite   = model.get_telescope();
    ncoeff  = model.get_ncoeff();
    freq    = model.get_freq();
    nspan   = model.get_nspan();
  }
  else {
    cout << "Warning: Using default values to build polyco" << endl;
    maxha   = 12;
    nsite   = ephemeris.value_str[EPH_TZRSITE][0];
    ncoeff  = 12;
    freq    = get_centre_frequency();
    nspan   = 960;
  }

  if (clear_model)
    model = polyco();

  if ( model.i_nearest (time) == -1 ) {
    // no match, create a new polyco for the specified time
    polyco part = Tempo::get_polyco (ephemeris, time, time,
				     nspan, ncoeff, maxha, nsite, freq);   
    model.append (part);
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
void Pulsar::Archive::apply_model (const polyco& old, Integration* subint)
{
  if ( model.get_telescope() != old.get_telescope() )
    throw Error (InvalidState, "Archive::apply_model", "telescope mismatch");

  try {

    if (verbose) 
      model.unload (stderr);

    // get the MJD of the rising edge of bin zero
    MJD subint_mjd = subint -> get_epoch();

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
      subint_mjd = subint -> get_epoch();
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

// ///////////////////////////////////////////////////////////////////////
//
// Archive::need_updated_model
//
/*!
  This method tests if the given model has a polynomial that applies
  to each Integration, returning false if a match is not found.
  \param test_model the polyco to be tested
 */
bool Pulsar::Archive::good_model (const polyco& test_model) const
{
  if (verbose)
    cerr << "Archive::good_model testing polyco on " << get_nsubint()
	 << " integrations" << endl;

  unsigned isub=0;
  for (isub=0; isub < get_nsubint(); isub++)
    try {
      if ( test_model.i_nearest (get_Integration(isub)->get_epoch()) == -1 )
	break;
    }
    catch (...) {
      break;
    }
  
  if (isub < get_nsubint()) {
    if (verbose)
      cerr << "Archive::good_model polyco failed on integration "
	   << isub << endl;
    return false;
  }
  
  return true;
}
