#include "Pulsar/Profile.h"

/*! This use of the Functor template implements the Strategy design
 pattern (ghjv94 p.315) for calculating the shift between an
 observation and a standard.  By default, the shift is calculated
 using the PhaseGradShift concrete strategy. */
Functor< Estimate<double>(Pulsar::Profile,Pulsar::Profile) > 
Pulsar::Profile::shift_strategy (&PhaseGradShift);

/*
  \param std the profile with respect to which the shift will be estimated
  \retval shift and its error in turns
*/
Estimate<double> Pulsar::Profile::shift (const Profile& std) const
{
  return shift_strategy (std, *this);
}

