/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/BaselineWindow.h"
#include "Pulsar/PhaseWeightShift.h"
#include "Pulsar/Dispersion.h"

#include "Error.h"

using namespace std;

Pulsar::PhaseWeight* Pulsar::Integration::baseline () const
{
  Reference::To<const Integration> total = this->total();
  return total->get_Profile(0,0)->baseline ();
}

static Pulsar::PhaseWeightShift shift;
static Pulsar::Dispersion dispersion;

static int static_init ()
{
  Functor<double()> get_shift ( &dispersion, &Pulsar::Dispersion::get_shift );
  shift.get_shift = get_shift;
  return 0;
}

static int initalize = static_init ();

/*!
  If baseline is not specified, this method calls
  Integration::baseline to find the baseline mask.

  This mask is then used to remove the baseline from each of the
  profiles over all polarizations and frequencies.  If the dispersion
  measure and folding period have been previously set, the baseline
  phase is shifted according to the dispersion relation.
*/
void Pulsar::Integration::remove_baseline (const PhaseWeight* baseline) try 
{
  if (verbose)
    cerr << "Pulsar::Integration::remove_baseline ("<< baseline <<")" << endl;

  Reference::To<PhaseWeight> my_baseline;
  if (!baseline)
    baseline = my_baseline = this->baseline();

  // set the input to the PhaseWeight shifter
  shift.set_weight( baseline );

  // set up the dispersion computation attributes
  dispersion.set( this );

  // the output of the PhaseWeight shifter
  PhaseWeight shifted_baseline;

  for (unsigned ichan=0; ichan<get_nchan(); ichan++) {

    if (verbose)
      cerr << "Pulsar::Integration::remove_baseline ichan=" << ichan << endl;

    // compute the dispersion shift for this channel (input to shifter)
    dispersion.set_Profile( profiles[0][ichan] );

    // get the shifted PhaseWeight mask
    shift.get_weight( &shifted_baseline );

    for (unsigned ipol=0; ipol<get_npol(); ipol++) {

      if (verbose)
	cerr << "Pulsar::Integration::remove_baseline ipol=" << ipol << endl;

      Profile* profile = get_Profile(ipol,ichan);

      double mean, variance;
      shifted_baseline.stats (profile, &mean, &variance);

      profile->offset (-mean);

    }

  }

}
catch (Error& error) {
  throw error += "Integration::remove_baseline";
}

//! Return the statistics of every profile baseline
void
Pulsar::Integration::baseline_stats (vector<vector<Estimate<double> > >* mean,
				     vector< vector<double> >* variance) const
try {

  if (verbose)
    cerr << "Pulsar::Integration::baseline_stats" << endl;

  // set the input to the PhaseWeight shifter
  shift.set_weight( this->baseline() );

  // set up the dispersion computation attributes
  dispersion.set( this );

  // the output of the PhaseWeight shifter
  PhaseWeight shifted_baseline;

  unsigned npol = get_npol();
  unsigned nchan = get_nchan();

  if (mean)
    mean->resize (npol);
  if (variance)
    variance->resize (npol);

  double* meanval_ptr = 0;
  double* meanvar_ptr = 0;
  double* variance_ptr = 0;

  for (unsigned ipol=0; ipol<npol; ++ipol) {

    if (mean)
      (*mean)[ipol].resize (nchan);
    if (variance)
      (*variance)[ipol].resize (nchan);
    
  }


  for (unsigned ichan=0; ichan<nchan; ++ichan) {

    if (get_weight(ichan) == 0) {

      if (verbose)
	cerr << "Pulsar::Integration::baseline_stats zero weight ichan="
	     << ichan << endl;

      for (unsigned ipol=0; ipol<npol; ++ipol) {
	if (mean)
	  (*mean)[ipol][ichan] = 0;
	if (variance)
	  (*variance)[ipol][ichan] = 0;
      }

      continue;

    }
    
    // compute the dispersion shift for this channel (input to shifter)
    dispersion.set_Profile( profiles[0][ichan] );

    // get the shifted PhaseWeight mask
    shift.get_weight( &shifted_baseline );

    for (unsigned ipol=0; ipol<npol; ipol++) {

      if (mean) {
	meanval_ptr = &((*mean)[ipol][ichan].val);
	meanvar_ptr = &((*mean)[ipol][ichan].var);
      }
      if (variance)
	variance_ptr = &((*variance)[ipol][ichan]);

      shifted_baseline.stats( get_Profile(ipol,ichan),
			      meanval_ptr, variance_ptr, meanvar_ptr );
      
    }

  }

  if (verbose)
    cerr << "Pulsar::Integration::baseline_stats exit" << endl;

}
catch (Error& error) {
  throw error += "Integration::baseline_stats";
}

