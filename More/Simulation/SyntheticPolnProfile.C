/***************************************************************************
 *
 *   Copyright (C) 2005-2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SyntheticPolnProfile.h"
#include "Pulsar/PolnProfile.h"
#include "MEAL/Axis.h"
#include "BoxMuller.h"
#include "random.h"

#include <vector>
#include <algorithm>

using namespace MEAL;

Pulsar::SyntheticPolnProfile::SyntheticPolnProfile ()
{
  noise_variance = 0;
  swims_variance = 0;
  baseline = 0;
  
  constant_degree = 0;
  constant_position_angle = 0;
}

Pulsar::SyntheticPolnProfile::~SyntheticPolnProfile ()
{
}

void
Pulsar::SyntheticPolnProfile::set_total_intensity (Univariate<Scalar>* f)
{
  total_intensity = f;
}

void Pulsar::SyntheticPolnProfile::set_degree (Univariate<Scalar>* f)
{
  degree = f;
}

void Pulsar::SyntheticPolnProfile::set_degree (float c)
{
  constant_degree = c;
  degree = 0;
}

void Pulsar::SyntheticPolnProfile::set_noise_variance (float c)
{
  noise_variance = c;
}

void Pulsar::SyntheticPolnProfile::set_noise (float rms)
{
  noise_variance = rms * rms;
}

void Pulsar::SyntheticPolnProfile::set_swims_variance (float c)
{
  swims_variance = c;
}

void Pulsar::SyntheticPolnProfile::set_swims (float rms)
{
  swims_variance = rms * rms;
}


void Pulsar::SyntheticPolnProfile::set_baseline (float c)
{
  baseline = c;
}


void
Pulsar::SyntheticPolnProfile::set_position_angle (Univariate<Scalar>* f)
{
  position_angle = f;
}

void Pulsar::SyntheticPolnProfile::set_position_angle (float c)
{
  constant_position_angle = c;
  position_angle = 0;
}

void 
Pulsar::SyntheticPolnProfile::set_linear (Univariate<Scalar>* f)
{
  linear = f;
}

void Pulsar::SyntheticPolnProfile::set_circular (Univariate<Scalar>* f)
{
  circular = f;
}

void Pulsar::SyntheticPolnProfile::set_Profile (Pulsar::Profile* profile, Univariate<Scalar>* function)
{
  unsigned nbin = profile->get_nbin();

  Axis<double> argument;  
  function->set_argument (0, &argument);

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    argument.set_value( (2*M_PI*ibin)/nbin );
    //    function->set_abscissa ( (2*M_PI*ibin)/nbin );
    profile->get_amps()[ibin] = function->evaluate();
  }
}


Pulsar::PolnProfile* 
Pulsar::SyntheticPolnProfile::get_PolnProfile (unsigned nbin) const
{
  Reference::To<PolnProfile> result = new PolnProfile (nbin);
  // result->convert_state (Signal::Stokes);
  get_PolnProfile (result.get());
  return result.release();
}

void Pulsar::SyntheticPolnProfile::get_PolnProfile (PolnProfile* result) const
{
  unsigned nbin = result->get_nbin();

  Profile* I = result->get_Profile (0);
  Profile* Q = result->get_Profile (1);
  Profile* U = result->get_Profile (2);
  Profile* V = result->get_Profile (3);

  if (circular)
    set_Profile (V, circular);
  else
    V->zero ();

  if (linear)
    set_Profile (Q, linear);
  else
    Q->zero ();

  U->zero ();

  if (total_intensity)
  {
    set_Profile (I, total_intensity);

    if (!linear && (degree || constant_degree))
    {
      float poln_deg = 0.0;
      for (unsigned ibin=0; ibin<nbin; ibin++)
      {
	if (degree)
	{
	  degree->set_abscissa ( (2*M_PI*ibin)/nbin );
	  poln_deg = degree->evaluate();
	}
	else
	  poln_deg = constant_degree;
	
        float p = I->get_amps()[ibin] * poln_deg;
	float v = V->get_amps()[ibin];
	Q->get_amps()[ibin] = sqrt(p*p-v*v);
      }

    } // if not linear and degree

  } // if total_intensity

  else if (degree || constant_degree)
  {
    float poln_deg = 0.0;
    for (unsigned ibin=0; ibin<nbin; ibin++)
    {
      if (degree)
      {
	degree->set_abscissa ( (2*M_PI*ibin)/nbin );
	poln_deg = degree->evaluate();
      }
      else
	poln_deg = constant_degree;

      I->get_amps()[ibin] = result->get_Stokes(ibin).abs_vect() / poln_deg;
    }
  }
  else
    throw Error (InvalidState, "Pulsar::SyntheticPolnProfile::get_Profile",
		 "no means of calculating the total intensity profile");

  if (position_angle || constant_position_angle)
  {
    float PA = 0.0;
    for (unsigned ibin=0; ibin<nbin; ibin++)
    {
      if (position_angle)
      {
	position_angle->set_abscissa ( (2*M_PI*ibin)/nbin );
	PA = position_angle->evaluate();
      }
      else
	PA = constant_position_angle;

      float L = Q->get_amps()[ibin];
      Q->get_amps()[ibin] = L * cos(2*PA);
      U->get_amps()[ibin] = L * sin(2*PA);

    }
  }

  if (swims_variance)
  {
    static BoxMuller gasdev( usec_seed() );
    float rms = sqrt(swims_variance) / nbin;
    
    for (unsigned ipol=0; ipol<4; ipol++)
    {
      std::vector<float> noise (nbin);
      std::generate (noise.begin(), noise.end(), gasdev);

      float* amps = result->get_Profile(ipol)->get_amps();

      std::vector<float> correlated_noise (nbin, 0.0);

      for (unsigned ibin=0; ibin<nbin; ibin++)
	for (unsigned jbin=0; jbin<nbin; jbin++)
	  correlated_noise[ibin] += rms * amps[ibin] * amps[jbin] * noise[jbin];

      for (unsigned ibin=0; ibin<nbin; ibin++)
      {
	amps[ibin] += correlated_noise[ibin];
      }
    }
  }

  if (noise_variance)
  {
    static BoxMuller gasdev( usec_seed() );

    float rms = sqrt(noise_variance);

    for (unsigned ipol=0; ipol<4; ipol++)
    {
      Profile* p = result->get_Profile (ipol);
      for (unsigned ibin=0; ibin<nbin; ibin++)
	p->get_amps()[ibin] += rms * gasdev ();
    }
  }

  if (baseline)
  {
    float* amps = result->get_Profile(0)->get_amps();
    for (unsigned ibin=0; ibin<nbin; ibin++)
      amps[ibin] += baseline;
  }
}
