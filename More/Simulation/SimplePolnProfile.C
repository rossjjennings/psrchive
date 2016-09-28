/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SimplePolnProfile.h"
#include "Pulsar/PolnProfile.h"
#include "MEAL/ScalarParameter.h"

#include <iostream>

using namespace std;

Pulsar::SimplePolnProfile::SimplePolnProfile ()
{
  // generate svm total intensity profiles
  set_total_intensity (&svm);

  svm.set_width  (0.1*M_PI);
  svm.set_height (1.0);

  // generate linear polarization according to RVM
  set_position_angle (&rvm);

  rvm.line_of_sight->set_value (83.0*M_PI/180);
  rvm.magnetic_axis->set_value (89.0*M_PI/180);
  rvm.reference_position_angle->set_value (M_PI/4);

  set_centre (M_PI);

  // set degree of polarization
  set_degree (0.4);

  set_noise (0.01);
}

Pulsar::SimplePolnProfile::~SimplePolnProfile ()
{
}

//! Set the phase of the standard profile centre
void Pulsar::SimplePolnProfile::set_centre (double centre)
{
  svm.set_centre (centre);
  rvm.magnetic_meridian->set_value (centre);
}
