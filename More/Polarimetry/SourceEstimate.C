/***************************************************************************
 *
 *   Copyright (C) 2003 - 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SourceEstimate.h"
#include "Pulsar/Calibrator.h"

#include <iostream>
using namespace std;

//! Construct with the specified bin from Archive
Calibration::SourceEstimate::SourceEstimate (int ibin)
{
  phase_bin = ibin;
  input_index = 0;
  add_data_attempts = 0;
  add_data_failures = 0;
}

using Calibration::ReceptionModel;

void Calibration::SourceEstimate::create_source (ReceptionModel* equation)
{
  source = new MEAL::Coherency;

  input_index = equation->get_num_input();

  equation->add_input( source );
}


bool Calibration::SourceEstimate::is_constrained () const
{
  return add_data_attempts > add_data_failures;
}

void Calibration::SourceEstimate::report_input_failed (std::ostream& out) const
{
  string name = "uninitialized";
  if (source)
    name = source->get_param_name_prefix();

  out << "add data failed " << add_data_failures << " out of "
      << add_data_attempts << " times for " << name << endl;
}

/*! Update the best guess of each unknown input state */
void Calibration::SourceEstimate::update ()
{
  valid = true;

  try 
  {
    update_work ();
  }
  catch (Error& error)
  {
    if (Pulsar::Calibrator::verbose > 2)
      cerr << "Calibration::SourceEstimate::update_source error "
	   << error << endl;
    valid = false;
  }
}

/*! Update the best guess of each unknown input state */
void Calibration::SourceEstimate::update_work ()
{
  estimate.update( source );
}
