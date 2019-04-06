/***************************************************************************
 *
 *   Copyright (C) 2006 - 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DeltaPA.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/PhaseWeight.h"

#include "templates.h"

#include "stdio.h"
#include <iostream>
using namespace std;

template <typename T, typename U = T>
class MeanArc
{

public:

  //! Default constructor
  MeanArc () { covar = 0; }

  //! Copy constructor
  MeanArc (const MeanArc& mean)
  { operator = (mean); }

  //! Assignment operator
  const MeanArc& operator= (const MeanArc& mean)
  {
    cosine = mean.cosine;
    sine = mean.sine;
    covar = mean.covar;
    return *this;
  }

  //! Addition operator
  const MeanArc& operator+= (const MeanArc& mean)
  {
    cosine += mean.cosine;
    sine += mean.sine;
    covar += mean.covar;
    return *this;
  }

  //! Add data
  void add (const Estimate<T,U>& x0, const Estimate<T,U>& y0, 
            const Estimate<T,U>& x1, const Estimate<T,U>& y1,
	    double scale = 1.0)
  {
    Estimate<T,U> cosarc = x0*x1 + y0*y1;
    Estimate<T,U> sinarc = x0*y1 - y0*x1;

    // Goodman corrections
    cosarc.var += x0.var*x1.var + y0.var*y1.var;
    sine.var += x0.var*y1.var + y0.var*x1.var;

    cosine += scale * cosarc;
    sine += scale * sinarc;

    // del cosine del cosarc * del sine del sinarc
    T norm = scale*scale;

    // del cosarc del x0 * del sinarc del x0 * var(x0)
    covar += x1.get_value() * y1.get_value() * x0.get_variance() * norm;
 
    // del cosarc del y0 * del sinarc del y0 * var(y0)
    covar -= y1.get_value() * x1.get_value() * y0.get_variance() * norm;
 
    // del cosarc del x1 * del sinarc del x1 * var(x1)
    covar -= x0.get_value() * y0.get_value() * x1.get_variance() * norm;
 
    // del cosarc del y1 * del sinarc del y1 * var(y1)
    covar += y0.get_value() * x0.get_value() * y1.get_variance() * norm;

  }

  //!
  Estimate<T,U> get_Estimate () const
  {
    Estimate<T,U> arc;
    double C = cosine.val;
    double S = sine.val;
    double var_C = cosine.var;
    double var_S = sine.var;

    arc.val = atan2 (sine.val, cosine.val);

    double one = C*C + S*S;
    arc.var = ( C*C*var_S + S*S*var_C - 2.0*C*S*covar ) / (one*one);

    return arc;
  }

protected:

  Estimate<T,U> cosine;
  Estimate<T,U> sine;

  //! The covariance
  T covar;

};

//! Set the phase bins to be included in the mean
void Pulsar::DeltaPA::set_include (const std::vector<unsigned>& bins)
{
  include_bins = bins;
}

//! Set the phase bins to be excluded exclude the mean
void Pulsar::DeltaPA::set_exclude (const std::vector<unsigned>& bins)
{
  exclude_bins = bins;
}

#include "Pulsar/GaussianBaseline.h"
#include "Pulsar/PhaseWeight.h"

Estimate<double> 
Pulsar::DeltaPA::get (const PolnProfile* p0, const PolnProfile* p1) const
{
  if (!p0 || !p1)
    throw Error (InvalidState, "Pulsar::DeltaPA::get", "no data");

  Profile linear0;
  p0->get_linear (&linear0);

  Profile linear1;
  p1->get_linear (&linear1);

  Pulsar::GaussianBaseline mask;
  Pulsar::PhaseWeight weight;
    
  mask.set_Profile (p0->get_Profile(0));
  mask.get_weight (&weight);

  double mu_q0 = 0.0, var_q0 = 0.0;
  weight.stats (p0->get_Profile(1), &mu_q0, &var_q0);
  double mu_u0 = 0.0, var_u0 = 0.0;
  weight.stats (p0->get_Profile(2), &mu_u0, &var_u0);

  double mu_q1 = 0.0, var_q1 = 0.0;
  weight.stats (p1->get_Profile(1), &mu_q1, &var_q1);
  double mu_u1 = 0.0, var_u1 = 0.0;
  weight.stats (p1->get_Profile(2), &mu_u1, &var_u1);

  float cutoff0 = threshold * sqrt (0.5*(var_q0 + var_u0));
  float cutoff1 = threshold * sqrt (0.5*(var_q1 + var_u1));

#ifdef _DEBUG
  cerr << "1: var q=" << var_q1 << " u=" << var_u1 << " cut=" 
       << cutoff1 << endl;
#endif

  const float *q0 = p0->get_Profile(1)->get_amps(); 
  const float *u0 = p0->get_Profile(2)->get_amps(); 

  const float *q1 = p1->get_Profile(1)->get_amps(); 
  const float *u1 = p1->get_Profile(2)->get_amps(); 

  unsigned nbin = p0->get_nbin();
  used_bins = 0;

  FILE* fptr = fopen ("delta_pa.txt", "w");

  if (onpulse_weights && onpulse_weights->get_nbin() != nbin)
    throw Error (InvalidState, "Pulsar::DeltaPA::get",
		 "onpulse weight nbin != data nbin");

  MeanArc<double> arc;

  for (unsigned ibin=0; ibin<nbin; ibin++)
  {
    if (include_bins.size() && !found (ibin, include_bins))
      continue;

    if (exclude_bins.size() && found (ibin, exclude_bins))
      continue;

    double scale = 1.0;
    if (onpulse_weights)
      scale = (*onpulse_weights)[ibin];

    else if (threshold != 0 &&
	     ( linear0.get_amps()[ibin] < cutoff0 ||
	       linear1.get_amps()[ibin] < cutoff1) )
      continue;

    // cerr << "ibin=" << ibin << " scale=" << scale << endl;
    
    MeanArc<double> arc1;
    arc1.add( Estimate<double> (q0[ibin], var_q0),
	      Estimate<double> (u0[ibin], var_u0),
	      Estimate<double> (q1[ibin], var_q1),
	      Estimate<double> (u1[ibin], var_u1), scale );

    Estimate<double> delta_pa = arc1.get_Estimate();

    if (fptr)
      fprintf (fptr, "%u  %lf  %lf\n", ibin,
	       delta_pa.get_value(), delta_pa.get_error());

#ifdef _DEBUG
    cerr << "ibin=" << ibin << " dPA=" << delta_pa << endl;
#endif

    arc += arc1; 

    used_bins ++;
  }

  if (fptr)
    fclose (fptr);

  if (used_bins == 0)
    throw Error( InvalidParam, "Pulsar::DeltaPA::get",
		 "linear polarization did not exceed thresholds\n\t"
		 "threshold=%f  cutoff0=%f  cutoff1=%f",
		 threshold, cutoff0, cutoff1);

  return 0.5 * arc.get_Estimate();

}
