#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

Pulsar::PhaseWeight::PhaseWeight ()
{
}

Pulsar::PhaseWeight::PhaseWeight (const PhaseWeight& pm)
{
  weight = pm.weight;
}

Pulsar::PhaseWeight::PhaseWeight (const vector<float>& _weight)
{
  weight = _weight;
}

Pulsar::PhaseWeight::~PhaseWeight ()
{
}

const Pulsar::PhaseWeight& 
Pulsar::PhaseWeight::operator = (const PhaseWeight& pm)
{
  weight = pm.weight;
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
    weight [ipt] += pm.weight [ipt];

  return *this;
}

const Pulsar::PhaseWeight& 
Pulsar::PhaseWeight::operator *= (const PhaseWeight& pm)
{
  if (weight.size() != pm.weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::operator *=",
		 "weight size=%d != other weight size=%d",
		 weight.size(), pm.weight.size());

  for (unsigned ipt=0; ipt<weight.size(); ipt++)
    weight [ipt] *= pm.weight [ipt];

  return *this;
}

void Pulsar::PhaseWeight::set_all (float value)
{
  for (unsigned ipt=0; ipt<weight.size(); ipt++)
    weight [ipt] *= value;
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

void Pulsar::PhaseWeight::stats (const Profile* profile,
				 double* mean, double* var, double* varmean)
{
  if (Profile::verbose) cerr << "Pulsar::PhaseWeight::stats" << endl;
  
  double totwt = 0; 
  double tot   = 0;
  double totsq = 0;

  unsigned nbin = profile->get_nbin();

  if (nbin != weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::stats",
		 "weight size=%d != profile nbin=%d",
		 weight.size(), nbin);

  const float* amps = profile->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++) {
    double value = double(weight[ibin]) * double(amps[ibin]);
    tot   += value;
    totsq += value*value;
    totwt += weight[ibin];
  }

  double mean_x   = tot / totwt;
  double mean_xsq = totsq / totwt;
  double var_x = (mean_xsq - mean_x*mean_x) * totwt/(totwt-1.0);

  if (mean)
    *mean = mean_x;
  if (var)
    *var  = var_x;
  if (varmean)
    *varmean = var_x / totwt;

  if (Profile::verbose) cerr << "Pulsar::PhaseWeight::stats return" << endl;
}
