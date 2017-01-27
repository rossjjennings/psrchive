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

// #define _DEBUG 1
template <typename T, typename U=T>
class WeightedMeanEstimate
{
  //! The weighted total of all Estimate values
  T weighted_val;
  //! The weighted total of all Estimate variances
  U weighted_var;
  //! The sum of all the weights
  U sum_of_weights;

 public:

  WeightedMeanEstimate ()
  {
    weighted_val = 0;
    weighted_var = 0;
    sum_of_weights = 0;
  }

  bool valid () const { return sum_of_weights > 0; }
  
  //! Addition operator
  const WeightedMeanEstimate& operator+= (const WeightedMeanEstimate& d)
  {
    weighted_val += d.weighted_val;
    weighted_var += d.weighted_var;
    sum_of_weights += d.sum_of_weights;
    return *this;
  }

  //! Addition operator
  void add (double scale, const Estimate<T,U>& d)
  {
    if (d.var == 0)
      return;

    double weight = scale / d.var;
    sum_of_weights += weight;
    weighted_val += weight * d.val;
    weighted_var += weight * weight * d.var;
  }

  Estimate<T,U> get_Estimate () const
  {
    return Estimate<T,U> (weighted_val / sum_of_weights,
			  weighted_var / (sum_of_weights * sum_of_weights) );
  }

  double get_sum_of_weights () const { return sum_of_weights; }
};




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
    cosine.add (scale,cosarc);
    
    Estimate<T,U> sinarc = x0*y1 - y0*x1;
    sine.add (scale,sinarc);

    // del cosine del cosarc * del sine del sinarc
    T norm = scale*scale / (cosarc.get_variance() * sinarc.get_variance());

    // del cosarc del x0 * del sinarc del x0 * var(x0)
    covar += x1.get_value() * y1.get_value() * x0.get_variance() * norm;
 
    // del cosarc del y0 * del sinarc del y0 * var(y0)
    covar -= y1.get_value() * x1.get_value() * y0.get_variance() * norm;
 
    // del cosarc del x1 * del sinarc del x1 * var(x1)
    covar -= x0.get_value() * y0.get_value() * x1.get_variance() * norm;
 
    // del cosarc del y1 * del sinarc del y1 * var(y1)
    covar += y0.get_value() * x0.get_value() * y1.get_variance() * norm;

#ifdef _DEBUG
    cerr << "CC=" << 1.0/cosine.inv_var << " SS=" << 1.0/sine.inv_var
	 << " CS=" << covar << " norm=" << norm << endl;
#endif

  }

  //!
  Estimate<T,U> get_Estimate () const
  {
    if (!(sine.valid() && cosine.valid()))
      return Estimate<T,U>(0,0);

    Estimate<T,U> sbar = sine.get_Estimate();
    Estimate<T,U> cbar = cosine.get_Estimate();

    Estimate<T,U> arc = atan2 (sbar, cbar);

    U norm = sine.get_sum_of_weights() * cosine.get_sum_of_weights();
    T covar_sbarcbar = covar / norm;
    
    T one = (sbar.val*sbar.val + cbar.val*cbar.val);

    T delarc_delsbar = cbar.get_value() / one;
    T delarc_delcbar = -sbar.get_value() / one; 

#if 0
    cerr << "MeanArc::get_Estimate arc.var=" << arc.var << " additional=" <<
      2.0 * covar_sbarcbar * delarc_delsbar * delarc_delcbar << endl;
#endif
    
    arc.var += 2.0 * covar_sbarcbar * delarc_delsbar * delarc_delcbar;
    
    return arc;
  }

  Estimate<T,U> get_sin () const
  { return sine.get_Estimate(); }

  Estimate<T,U> get_cos () const
  { return cosine.get_Estimate(); }

protected:

  //! The average cosine
  WeightedMeanEstimate<T,U> cosine;

  //! The average sine
  WeightedMeanEstimate<T,U> sine;

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

  //double cos_delta_PA = 0.0;
  //double sin_delta_PA = 0.0;

  MeanArc<double> arc;

  FILE* fptr = fopen ("delta_pa.txt", "w");

  if (onpulse_weights && onpulse_weights->get_nbin() != nbin)
    throw Error (InvalidState, "Pulsar::DeltaPA::get",
		 "onpulse weight nbin != data nbin");

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
