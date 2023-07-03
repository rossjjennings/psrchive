/***************************************************************************
 *
 *   Copyright (C) 2003 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ScalarTemplateMatching.h"
#include "Pulsar/FluctuationSpectrumStats.h"
#include "Pulsar/Profile.h"

#include "RealTimer.h"
#include "Pauli.h"
#include "FTransform.h"

// #define _DEBUG 1

#include <memory>

using namespace std;
using namespace Pulsar;

bool ScalarTemplateMatching::verbose = false;

//! Default constructor
ScalarTemplateMatching::ScalarTemplateMatching ()
{
  init ();
}

//! Copy constructor
ScalarTemplateMatching::ScalarTemplateMatching (const ScalarTemplateMatching& fit)
{
  init ();
  operator = (fit);
}

/*! This operator is under development and is not fit for use */
ScalarTemplateMatching&
ScalarTemplateMatching::operator = (const ScalarTemplateMatching& fit)
{
  choose_maximum_harmonic = fit.choose_maximum_harmonic;
  maximum_harmonic = fit.maximum_harmonic;
  n_harmonic = fit.n_harmonic;
  regions_set = fit.regions_set;

  return *this;
}

//! Destructor
ScalarTemplateMatching::~ScalarTemplateMatching ()
{
}

ScalarTemplateMatching* ScalarTemplateMatching::clone () const
{
  return new ScalarTemplateMatching (*this);
}

void ScalarTemplateMatching::init ()
{
  maximum_harmonic = n_harmonic = 0;
  regions_set = false;
  choose_maximum_harmonic = false;
  
  compute_reduced_chisq = true;

  stats = new FluctuationSpectrumStats;
}

void ScalarTemplateMatching::set_plan (FTransform::Plan* p)
{
  stats->set_plan (p);
}

void ScalarTemplateMatching::set_maximum_harmonic (unsigned max)
{
  maximum_harmonic = max;
}

void ScalarTemplateMatching::set_choose_maximum_harmonic (bool flag)
{
  choose_maximum_harmonic = flag;
}

//! Get the standard to which observations will be fit
const Profile* ScalarTemplateMatching::get_standard () const
{
  if (data.size() == 0)
    throw Error (InvalidState, "ScalarTemplateMatching::get_standard", "no data");

  return data[0].get_standard();
}

void ScalarTemplateMatching::set_regions (const PhaseWeight& on, const PhaseWeight& off)
{
  stats->set_regions ( on, off );
  regions_set = true;
}

//! Set the standard to which observations will be fit
void ScalarTemplateMatching::set_standard (const Profile* _standard)
{
  if (!_standard)
  {
    data.resize(0);
    return;
  }

  if (!regions_set)
  {
    DEBUG("ScalarTemplateMatching::set_standard select profile");
    stats->select_profile (_standard);
  }

  data.resize(1);
  data[0].set_stats(stats);
  data[0].set_standard(_standard);

  // number of complex phase bins in Fourier domain
  unsigned std_harmonic = _standard->get_nbin() / 2;

  DEBUG("ScalarTemplateMatching::set_standard max harmonic");

  if (choose_maximum_harmonic)
  {
    n_harmonic = stats->get_last_harmonic ();
    if (verbose)
      cerr << "ScalarTemplateMatching::set_standard chose " << n_harmonic << " harmonics" << endl;
  }
  else if (maximum_harmonic && maximum_harmonic < std_harmonic)
  {
    n_harmonic = maximum_harmonic;
    if (verbose)
      cerr << "ScalarTemplateMatching::set_standard using " << maximum_harmonic << " out of " << std_harmonic << " harmonics" << endl;
  }
  else
  {
    n_harmonic = std_harmonic;
    if (verbose)
      cerr << "ScalarTemplateMatching::set_standard using all " << std_harmonic << " harmonics" << endl;
  }

  DEBUG("ScalarTemplateMatching::set_standard created");
}

//! Fit the specified observation to the standard
void ScalarTemplateMatching::fit (const Profile* observation) try
{
  set_observation (observation);
  solve ();
}
catch (Error& error)
{
  throw error += "ScalarTemplateMatching::fit";
}

void ScalarTemplateMatching::delete_observations ()
{
  data.resize(0);
}

void ScalarTemplateMatching::set_observation (const Profile* obs) try
{
  if (!obs)
    throw Error (InvalidState, "ScalarTemplateMatching::set_observation",
		 "no observation supplied as argument");

  if (data.size() == 0)
    throw Error (InvalidState, "ScalarTemplateMatching::set_observation",
		 "no standard; call set_standard first");

  if (data.size() > 1)
    throw Error (InvalidState, "ScalarTemplateMatching::set_observation",
		 "unexpected data.size > 1");

  data[0].set_observation(obs);
}
catch (Error& error)
{
  throw error += "ScalarTemplateMatching::set_observation";
}

void ScalarTemplateMatching::solve () try
{
  RealTimer clock;

  clock.start();

  model_profile ();

  clock.stop();

  if (verbose)
  {
    cerr << "ScalarTemplateMatching::solve solved in " << clock << "."
      " chisq=" << chisq/nfree << endl;
  }
}
catch (Error& error) {
  throw error += "ScalarTemplateMatching::solve";
}

double ScalarTemplateMatching::get_reduced_chisq () const
{
  return chisq / nfree;
}

//! Get the phase offset between the observation and the standard
Estimate<double> ScalarTemplateMatching::get_phase () const
{
  return best_shift;
}


void ScalarTemplateMatching::Data::set_stats (FluctuationSpectrumStats* _stats)
{
  stats = _stats;
}

FluctuationSpectrumStats* ScalarTemplateMatching::Data::get_stats ()
{
  return stats;
}

void ScalarTemplateMatching::Data::set_standard (const Profile* std)
{
  standard.profile = std;
  stats->set_profile( std );
  standard.fourier = stats->get_fourier();
  standard.variance = stats->get_noise_variance();
}

const Profile* ScalarTemplateMatching::Data::get_standard () const
{
  return standard.profile;
}

const Profile* ScalarTemplateMatching::Data::get_standard_fourier () const
{
  return standard.fourier;
}

void ScalarTemplateMatching::Data::set_observation (const Profile* obs)
{
  observation.profile = obs;
  stats->set_profile( obs );
  observation.fourier = stats->get_fourier();
  observation.variance = stats->get_noise_variance();
}

const Profile* ScalarTemplateMatching::Data::get_observation () const
{
  return observation.profile;
}

const Profile* ScalarTemplateMatching::Data::get_observation_fourier () const
{
  return observation.fourier;
}

double ScalarTemplateMatching::Data::get_residual_variance (double scale) const
{
  return observation.variance + scale*scale*standard.variance;
}
