/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseWeight::PhaseWeight ()
{
  built = false;
}

Pulsar::PhaseWeight::PhaseWeight (const PhaseWeight& pm)
{
  weight = pm.weight;
  built = false;
}

Pulsar::PhaseWeight::PhaseWeight (const vector<float>& _weight)
{
  weight = _weight;
  built = false;
}

Pulsar::PhaseWeight::~PhaseWeight ()
{
}

const Pulsar::PhaseWeight& 
Pulsar::PhaseWeight::operator = (const PhaseWeight& pm)
{
  weight = pm.weight;
  built = false;
  return *this;
}

const Pulsar::PhaseWeight& 
Pulsar::PhaseWeight::operator += (const PhaseWeight& pm)
{
  if (weight.size() != pm.weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::operator +=",
		 "weight size=%d != other weight size=%d",
		 weight.size(), pm.weight.size());

  for (unsigned ipt=0; ipt<weight.size(); ipt++)
    weight[ipt] += pm.weight[ipt];

  built = false;
  return *this;
}

const Pulsar::PhaseWeight& 
Pulsar::PhaseWeight::operator *= (const PhaseWeight& pm)
{
  if (weight.size() != pm.weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::operator *=",
		 "weight size=%d != other weight size=%d",
		 weight.size(), pm.weight.size());

  // cerr << "Pulsar::PhaseWeight::operator *=" << endl;

  for (unsigned ipt=0; ipt<weight.size(); ipt++)
    weight[ipt] *= pm.weight[ipt];

  built = false;
  return *this;
}

void Pulsar::PhaseWeight::set_all (float value)
{
  for (unsigned ipt=0; ipt<weight.size(); ipt++)
    weight[ipt] = value;
  built = false;
}
 
//! Retrieve the weights
void Pulsar::PhaseWeight::get_weights (vector<float>& weights) const
{
  weights = weight;
}

double Pulsar::PhaseWeight::get_weight_sum () const
{
  double total = 0.0;
  for (unsigned i=0; i<weight.size(); i++)
    total += weight[i];
  return total;
}

double Pulsar::PhaseWeight::get_weight_max () const
{
  double max = 0.0;
  for (unsigned i=0; i<weight.size(); i++)
    if (weight[i] > max)
      max = weight[i];
  return max;
}

//! Set the Profile to which the weights apply
void Pulsar::PhaseWeight::set_Profile (const Profile* _profile)
{
  profile = _profile;
  built = false;
}

//! Get the weighted total of the Profile amplitudes
double Pulsar::PhaseWeight::get_weighted_sum () const
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::PhaseWeight::get_weighted_sum",
		 "Profile not set");

  unsigned nbin = profile->get_nbin();

  if (nbin != weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::stats",
		 "weight size=%d != profile nbin=%d",
		 weight.size(), nbin);

  const float* amps = profile->get_amps();

  double total = 0.0;
  for (unsigned ibin=0; ibin<weight.size(); ibin++)
    total += weight[ibin] * amps[ibin];

  return total;
}

//! Get the weighted mean of the Profile
Estimate<double> Pulsar::PhaseWeight::get_mean () const
{
  if (!built)
    const_cast<PhaseWeight*>(this)->build();
  return mean;
}

//! Get the weighted variance of the Profile
Estimate<double> Pulsar::PhaseWeight::get_variance () const
{
  if (!built)
    const_cast<PhaseWeight*>(this)->build();
  return variance;
}

void Pulsar::PhaseWeight::build ()
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::PhaseWeight::build",
		 "Profile not set");

  stats (profile, &(mean.val), &(variance.val), &(mean.var), &(variance.var));
  built = true;
}

//! Weigh the Profile amplitudes by the weights
void Pulsar::PhaseWeight::weight_Profile (Profile* data) const
{
  unsigned nbin = data->get_nbin();

  if (nbin != weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::stats",
		 "weight size=%d != profile nbin=%d",
		 weight.size(), nbin);

  float* amps = data->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++)
    amps[ibin] *= weight[ibin];
}

void Pulsar::PhaseWeight::stats (const Profile* profile,
				 double* mean, double* var,
				 double* varmean, double* varvar) const
{
  if (Profile::verbose) cerr << "Pulsar::PhaseWeight::stats" << endl;
  
  unsigned nbin = profile->get_nbin();
  unsigned ibin = 0;

  if (nbin != weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::stats",
		 "weight size=%d != profile nbin=%d",
		 weight.size(), nbin);

  const float* amps = profile->get_amps();

  double totwt = 0; 
  double mu   = 0;

  for (ibin=0; ibin < nbin; ibin++) {
    double value = double(weight[ibin]) * double(amps[ibin]);
    mu += value;
    totwt += weight[ibin];
  }

  // cerr << "weight=" << totwt << " sum=" << get_weight_sum() << endl;

  mu /= totwt;

  double mu2  = 0;
  double mu4  = 0;

  for (ibin=0; ibin < nbin; ibin++) {
    double value = double(weight[ibin]) * double(amps[ibin] - mu);
    double value2 = value * value;
    mu2  += value2;
    mu4  += value2*value2;
  }

  mu2 /= totwt;
  mu4 /= totwt;

  double totwt_1 = totwt - 1.0;
  double totwt3 = totwt * totwt * totwt;

  // bias-corrected sample variance
  double var_x = mu2 * totwt/totwt_1;
  double var_var = (totwt_1*totwt_1*mu4 - totwt_1*(totwt-3)*mu2*mu2)/totwt3;

  if (mean)
    *mean = mu;
  if (var)
    *var  = var_x;

  if (varmean)
    *varmean = var_x / totwt;
  if (varvar)
    *varvar = var_var;

  if (Profile::verbose) cerr << "Pulsar::PhaseWeight::stats return" << endl;
}
