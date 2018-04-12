/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FourierDomainFit.h"
#include "Pulsar/ProfileShiftFit.h"

using namespace std;

Pulsar::FourierDomainFit::FourierDomainFit ()
{
  use_mcmc = true;
  reduced_chisq = 0;
  snr = 0.0;
}

void Pulsar::FourierDomainFit::set_standard (const Profile* p)
{
  standard = p;
  fit.set_standard(standard);
  fit.set_nharm(standard->get_nbin()/4);  // Use half the harmonics
}

Estimate<double> Pulsar::FourierDomainFit::get_shift () const
{
  Profile prfcopy = *observation;

  Reference::To<Profile> obs_p = &prfcopy;

  if (use_mcmc)
    fit.set_error_method(ProfileShiftFit::MCMC_Variance);
  else
    fit.set_error_method(ProfileShiftFit::Traditional_Chi2);

  fit.set_Profile(obs_p);

  fit.compute();

  reduced_chisq = fit.get_reduced_chisq();
  snr = fit.get_snr();

  // Change shift range from 0->1 to -0.5->0.5
  Estimate<double> result = fit.get_shift();
  if (result.get_value() > 0.5) { result -= 1.0; }
  return result;
}

//! Return the statistical goodness of fit
double Pulsar::FourierDomainFit::get_reduced_chisq () const
{
  return reduced_chisq;
}

//! Return the S/N ratio
double Pulsar::FourierDomainFit::get_snr () const
{
  return snr;
}

//
// The following Interface defines the pat command-line options
//

using Pulsar::FourierDomainFit;

// Text interface to the FourierDomainFit class
class FourierDomainFit::Interface : public TextInterface::To<FourierDomainFit>
{
public:
  Interface (FourierDomainFit* = 0);
  std::string get_interface_name () const { return "FDM"; }
};

FourierDomainFit::Interface::Interface (FourierDomainFit* instance)
{
  if (instance)
    set_instance (instance);

  add( &FourierDomainFit::get_mcmc,
       &FourierDomainFit::set_mcmc,
       "mcmc", "Use Markov chain Monte Carlo to estimate uncertainty");

  add( &FourierDomainFit::get_iterations,
       &FourierDomainFit::set_iterations,
       "iter", "Number of iterations for MCMC uncertainty calculation");
}

TextInterface::Parser* FourierDomainFit::get_interface ()
{
  return new Interface(this);
}
