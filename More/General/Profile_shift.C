/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/PhaseGradShift.h"
#include "lazy.h"

using namespace std;

LAZY_STATIC(Pulsar::PhaseGradShift, shift_strategy, );

/*
  \param std the profile with respect to which the shift will be estimated
  \retval shift and its error in turns
*/
Estimate<double> Pulsar::Profile::shift (const Profile& std) const
{
  get_shift_strategy().set_standard (&std);
  get_shift_strategy().set_observation (this);
  return get_shift_strategy().get_shift ();
}

