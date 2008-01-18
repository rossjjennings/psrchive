/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

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
}

double VanVleck::d_out_pow(double in_pow)
{
}

double VanVleck::in_pow(double out_pow)
{
}

/* Theory for correlation correction functions:
 */

void VanVleck::set_in_power(double px, double py)
{
  pow_x=px;
  pow_y=py;
  // compute coeffs here
  compute_coeffs();
}

void VanVleck::correct_acf(std::vector<float> data)
{
}

void VanVleck::correct_ccf(std::vector<float> data)
{
}



