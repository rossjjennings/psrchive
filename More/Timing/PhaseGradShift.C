/***************************************************************************
 *
 *   Copyright (C) 2004 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseGradShift.h"
#include "Pulsar/ScalarTemplateMatching.h"
#include "Pulsar/Profile.h"

using namespace std;
using namespace Pulsar;

Pulsar::Option<bool>
Pulsar::PhaseGradShift::compute_reduced_chisq
(
  "PhaseGradientShift::compute_reduced_chisq", false,

  "Compute the reduced chisq of the residual profile",

  "When true, the reduced chisq is computed using the expected variance of the \n"
  "residual profile.  When false, it is assumed that the reduced chisq is unity, \n"
  "which is equivalent to assuming that the fit is perfect and the noise in the \n"
  "post-fit residual profile is equal to that of the observed profile. \n"
  "\n"
  "The default is false only for backward compatibility with previous studies. \n"
  "For further discussion, see the Appendix of Rogers et al (2023)."
);

//! Default constructor
PhaseGradShift::PhaseGradShift()
{
  stm = new ScalarTemplateMatching;
  stm->set_compute_reduced_chisq( compute_reduced_chisq );
}

//! Destructor
PhaseGradShift::~PhaseGradShift()
{
  
}

//! Set the maximum number of harmonics to include in fit
void PhaseGradShift::set_maximum_harmonic (unsigned max)
{
  stm->set_maximum_harmonic (max);
}

//! Allow software to choose the maximum harmonic
void PhaseGradShift::set_choose_maximum_harmonic (bool flag)
{
  stm->set_choose_maximum_harmonic (flag);
}

//! Set the profile with respect to which the shift will be estimated
void Pulsar::PhaseGradShift::set_standard (const Profile* std)
{
  stm->set_standard (std);
  ProfileStandardShift::set_standard (std);
}

Estimate<double> Pulsar::PhaseGradShift::get_shift () const
{
  // max float is of order 10^{38} - check that we won't exceed this
  // limiting factor in the DC term of the fourier transform

  if (standard->sum() > 1e18)
    throw Error (InvalidState, "Profile::PhaseGradShift", 
		 "standard DC=%lf > max float", standard->sum());

  if (observation->sum() > 1e18)
    throw Error (InvalidState, "Profile::PhaseGradShift", 
		 "profile DC=%lf > max float", observation->sum());

  const unsigned nbin = observation->get_nbin();

  if (Profile::verbose)
    cerr << "Profile::PhaseGradShift compare nbin=" << nbin << " " << standard->get_nbin() <<endl;

  stm->set_observation (observation);
  stm->solve();

  // ScalarTemplateMatching returns phase shift in radians
  // PhaseGradShift returns phase shift in turns
  return stm->get_phase () / (2*M_PI);
}

//! Return the statistical goodness of fit
double Pulsar::PhaseGradShift::get_reduced_chisq () const
{
  return stm->get_reduced_chisq();
}
