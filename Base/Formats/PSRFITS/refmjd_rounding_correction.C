/***************************************************************************
 *
 *   Copyright (C) 2015 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Predictor.h"
#include "polyco.h"

using namespace std;

/* This function is meant to fix GUPPI/PUPPI data that was processed (including
 * merging) with PSRCHIVE prior to the REF_MJD polyco rounding bug being fixed
 * (2014/11/27 git commit bfc0c26).  For these files, evaluating the model at
 * the epoch will give a non-zero phase, in an amount that equals the REF_MJD
 * rounding error (~1e-10 days).
 *
 * NOTE: If data of this sort have been tscrunched across mutiple polyco blocks
 * (so that the epoch and subint start are in different blocks), this error may
 * not be fixable.  Similarly, if the data have been rephased using a new
 * ephemeris since being recorded, the necessary information to fix the problem
 * is not available.  For these reasons, this fix should probably not be
 * automatically applied, instead it could be explicitly invoked, eg via the
 * fix interpreter.
 *
 * PBD 2015/01/14
*/

void Pulsar::FITSArchive::refmjd_rounding_correction()
{

  // Skip non-Pulsar files
  if (!model || get_type() != Signal::Pulsar)
    return;

  // Skip if not tempo1 polycos
  const polyco* polys = dynamic_cast<const polyco*>(model.get());
  if (!polys)
    return;

  // Additional checks that could help improve safety:
  //   - check for backend = ?UPPI
  //   - check history table for ephem install or tscrunch, etc
  //   - check filename for expected 'raw' format
  //   - check file date for known fix installation time

  for (unsigned isub=0; isub<get_nsubint(); isub++) 
  {
    Integration *subint = get_Integration(isub);
    MJD sub_epoch = subint->get_epoch();

    // Get the polyco set that applies to this subint
    polynomial poly = polys->best(sub_epoch);

    // Compute REF_MJD rounding error when cast to a double
    // How system-dependent is this?  Hopefully not very...
    // First turn MJD to double, then back to MJD:
    MJD ref_mjd_dbl = poly.get_reftime().in_days();
    // Subtract this from epoch if necessary:
    double error_sec = (ref_mjd_dbl - poly.get_reftime()).in_seconds();

    // Now compare with evaluation of model at epoch
    double phs = poly.phase(sub_epoch).fracturns();
    if (phs>0.5) phs -= 1.0;
    double per = poly.period(sub_epoch);
    double phs_sec = phs*per;

    //cerr << "isub=" << isub << " dt=" << error_sec*1e9 << "ns " 
    //  << "dphs=" << phs_sec*1e9 << "ns";

    // If these agree within 1 ns (ok cutoff?) apply the correction
    if (fabs(phs_sec-error_sec) < 1e-9)
    {
      sub_epoch -= error_sec;
      subint->set_epoch(sub_epoch);
      //cerr << " fixing";
    }

    //cerr << endl;

  }

}
