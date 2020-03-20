/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SourceDeltaEstimate.h"
#include "Pulsar/Calibrator.h"

#include <iostream>
using namespace std;

using Calibration::ReceptionModel;

void Calibration::SourceDeltaEstimate::create_source (ReceptionModel* equation)
{
  source = new MEAL::Coherency;

  total = new MEAL::SumRule< MEAL::Complex2 >;
  total->add_model (source);
  
  input_index = equation->get_num_input();

  equation->add_input( total );
}

bool Calibration::SourceDeltaEstimate::has_baseline () const
{
  return baseline;
}

bool Calibration::SourceDeltaEstimate::has_total () const
{
  return total;
}

//! Set the baseline to which the delta is added
void Calibration::SourceDeltaEstimate::set_baseline (SourceEstimate* base) try
{
  if (baseline)
    throw Error (InvalidState, "Calibration::SourceDeltaEstimate::set_baseline",
		 "baseline already set");
  
  baseline = base;
  total->add_model (baseline->source);
}
 catch (Error& error)
   {
     throw error += "Calibration::SourceDeltaEstimate::set_baseline";
   }

/*! Update the best guess of each unknown input state */
void Calibration::SourceDeltaEstimate::update_work ()
{
  if (!baseline)
    throw Error (InvalidState, "Calibration::SourceDeltaEstimate::update_work",
		 "baseline not set");

  Stokes< Estimate<double> > Sbaseline = baseline->estimate.get_mean ();
  Stokes< Estimate<double> > Stotal = estimate.get_mean ();
  Stokes< Estimate<double> > Sdelta = Stotal - Sbaseline;
  
  source->set_stokes ( Sdelta );
}
