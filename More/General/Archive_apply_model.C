/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Predictor.h"

#include <iostream>
using namespace std;

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
void Pulsar::Archive::apply_model (Integration* subint, const Predictor* old)
{
  if ( !model )
    throw Error (InvalidState, "Pulsar::Archive::apply_model",
		 "no Predictor");

  try {

    // get the MJD of the rising edge of bin zero
    MJD epoch = subint -> get_epoch();

    // get the observing frequency
    double freq = subint -> get_centre_frequency ();

    // get the phase shift due to differing reference frequencies
    Phase freq_shift_phase = 0;

    if (old)
      freq_shift_phase = model->dispersion (epoch, freq) 
	- old->dispersion (epoch, freq);

    // get the phase of the rising edge of bin zero
    Phase phase = model->phase (epoch);
    
    // the Integration is rotated by -phase to bring zero phase to bin zero
    Phase dphase = freq_shift_phase - phase;
    
    long double period = 1.0 / model->frequency (epoch);
    long double shift_time = dphase.fracturns() * period;
    
    if (verbose == 3) {

      Phase old_phase = (old) ? old->phase(epoch) : 0;

      cerr << "Pulsar::Archive::apply_model"
	   << "\n  old MJD " << epoch;

      if (old)
	cerr << "\n  old predictor phase " << old_phase;
	  //	     << "\n  old freq " << old->get_freq();

      cerr << "\n  new predictor phase " << phase
	//	   << "\n  new freq " << model->get_freq()
	   << "\n  freq phase shift " << freq_shift_phase
	
	   << "\n  time shift " << shift_time/86400.0 << " days" 
           << "\n             " << shift_time << " seconds "
	   << "\n  total phase shift " << dphase << endl; 
    }

    subint -> set_folding_period (period);  
    subint -> rotate (shift_time);
    subint -> zero_phase_aligned = true;

    if (verbose == 3) {
      epoch = subint -> get_epoch();
      phase = model->phase(epoch);
      cerr << "Pulsar::Archive::apply_model"
	   << "\n  new MJD "   << epoch
	   << "\n  new phase " << phase
	   << endl;

    }
  }
  catch (Error& err) {
    throw err += "Pulsar::Archive::apply_model";
  }

}
