/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <math.h>
#include "VanVleck.h"
#include "NormalDistribution.h"
#include "Error.h"

VanVleck::VanVleck()
{
  nlevel=0;
  pow_x=-1.0;
  pow_y=-1.0;
}

void VanVleck::set_nlevel(int n)
{
  if (n<2) 
    throw Error (InvalidParam, "VanVleck::set_nlevel",
        "Invalid nlevel=%d", n);
  if (n%2) { // Odd N
    // Here we assume center level is always 0.0
    thresh.resize((n-1)/2);
    levels.resize((n-1)/2);
  } else { // Even N
    // Here we assume center thresh is always 0.0
    thresh.resize(n/2 - 1);
    levels.resize(n/2);
  }
  nlevel = n;
}

void VanVleck::set_uniform_threshold() 
{
  float t;
  if (nlevel%2) { t=0.5; } 
  else { t=1.0; }
  for (int i=0; i<thresh.size(); i++) { thresh[i]=t; t+=1.0; }
}

void VanVleck::set_threshold(float *thr) 
{
}

void VanVleck::set_uniform_levels()
{
  float l;
  if (nlevel%2) { l=1.0; }
  else { l=0.5; }
  for (int i=0; i<levels.size(); i++) { levels[i]=l; l+=1.0; }
}

void VanVleck::set_canonical_levels()
{
  if (nlevel==2) { 
    levels[0] = 1.0; // Probably not used much for pulsar data..
  } else if (nlevel==3) {
    levels[0] = 1.0; // Only 1 non-zero level in this case
  } else if (nlevel==4) {
    levels[0] = 1.0; 
    levels[1] = 3.0; // Classic 2-bit output levels
  } else {
    set_uniform_levels(); // All others get uniform levels
  }
}

void VanVleck::set_levels(float *lev) 
{
}

/* Theory for total power correction functions:
 */

double VanVleck::out_pow(double in_pow)
{
  double sum=0.0;
  NormalDistribution n;
  const double in_pow_1_2 = sqrt(in_pow);

  // Check for in_pow=0
  if (in_pow==0.0) { 
      if (nlevel%2) return(0.0); 
      else return(levels[0]*levels[0]); 
  }

  // Pre-calculate cumulatve dist values at thresholds.
  std::vector<double> cdist(thresh.size());
  for (int i=0; i<thresh.size(); i++) { 
      cdist[i] = n.cumulative_distribution(thresh[i]/in_pow_1_2);
  }

  if (nlevel%2) { // Odd nlevel
    for (int i=0; i<levels.size()-1; i++) {
      sum += levels[i] * levels[i] * (cdist[i+1] - cdist[i]);
    }
    sum += levels[levels.size()-1] * levels[levels.size()-1] *
      (1.0 - cdist[levels.size()-1]);
    sum *= 2.0;
  } else {        // Even nlevel
    sum += levels[0]*levels[0]*(cdist[0] - 0.5);
    for (int i=1; i<levels.size()-1; i++) {
      sum += levels[i] * levels[i] * (cdist[i] - cdist[i-1]);
    }
    sum += levels[levels.size()-1] * levels[levels.size()-1] * 
      (1.0 - cdist[levels.size()-2]);
    sum *= 2.0;
  }

  return(sum);
}

double VanVleck::d_out_pow(double in_pow)
{
  double sum=0.0;
  NormalDistribution n;
  const double in_pow_1_2 = sqrt(in_pow);
  const double in_pow_3_2 = in_pow_1_2 * in_pow;

  // Check for in_pow=0
  if (in_pow==0.0) { return(0.0); }

  // Pre-calculate density values at thresholds.
  std::vector<double> dens(thresh.size());
  for (int i=0; i<thresh.size(); i++) {
    dens[i] = n.density(thresh[i]/in_pow_1_2) * 
      (-0.5 * thresh[i]/in_pow_3_2);
  }

  if (nlevel%2) { // Odd
    for (int i=0; i<levels.size(); i++) {
      sum += levels[i] * levels[i] * (dens[i+1] - dens[i]);
    }
    sum += levels[levels.size()-1] * levels[levels.size()-1] *
      (-dens[levels.size()-1]);
    sum *= 2.0;
  } else {        // Even
    sum += levels[0]*levels[0]*dens[0];
    for (int i=1; i<levels.size()-1; i++) {
      sum += levels[i] * levels[i] * (dens[i] - dens[i-1]);
    }
    sum += levels[levels.size()-1] * levels[levels.size()-1] * 
      (-dens[levels.size()-2]);
    sum *= 2.0;
  }

  return(sum);
}

// Uses Newton's method and the previous two functions to 
// invert the power relationship.
double VanVleck::in_pow(double out_pow_meas)
{

  // If out of range, return negative.
  // Or should we throw a error here?
  const double pmax = levels.back()*levels.back();
  const double pmin = (nlevel%2)?0.0:levels.front()*levels.front();
  if (out_pow_meas<=pmin) { return(0.0); }
  if (out_pow_meas>=pmax) { return(-1.0); }

  // Newton parameters
  const int max_it = 100;
  const double frac_tol = 1e-10;

  // Initialize on some linear scale
  double result = ((out_pow_meas-pmin)/(pmax-pmin)+0.1) * thresh[0] * thresh[0];

  // Iterate
  int nit=0;
  double delta=0.0;
  while (nit<max_it) {
      delta = (out_pow_meas-out_pow(result))/d_out_pow(result);
      if (fabs(delta/result) < frac_tol) { return(result); }
      result += delta;
      nit++;
  }

  // Maxed out iterations
  return(-2.0);
}

/* Theory for correlation correction functions:
 */

void VanVleck::set_in_power(double px, double py)
{
  pow_x=px;
  pow_y=py;
  // compute coeffs here
  //compute_coeffs();
}

void VanVleck::correct_acf(std::vector<float> data)
{
}

void VanVleck::correct_ccf(std::vector<float> data)
{
}



