/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DeltaPA.h"
#include "Pulsar/PolnProfile.h"

#include <iostream>
using namespace std;

// #define _DEBUG 1

Estimate<double> 
Pulsar::DeltaPA::get (const PolnProfile* p0, const PolnProfile* p1) const
{
  if (!p0 || !p1)
    throw Error (InvalidState, "Pulsar::DeltaPA::get", "no data");

  Profile linear0;
  p0->get_linear (&linear0);

  Profile linear1;
  p1->get_linear (&linear1);

  float min_phase = (linear0.find_min_phase() + linear1.find_min_phase()) / 2;

  double var_q0 = p0->get_variance (1, min_phase);
  double var_u0 = p0->get_variance (2, min_phase);
  float cutoff0 = threshold * sqrt (0.5*(var_q0 + var_u0));

  double var_q1 = p1->get_variance (1, min_phase);
  double var_u1 = p1->get_variance (2, min_phase);
  float cutoff1 = threshold * sqrt (0.5*(var_q1 + var_u1));

  const float *q0 = p0->get_Profile(1)->get_amps(); 
  const float *u0 = p0->get_Profile(2)->get_amps(); 

  const float *q1 = p1->get_Profile(1)->get_amps(); 
  const float *u1 = p1->get_Profile(2)->get_amps(); 

  unsigned nbin = p0->get_nbin();

  double cos_delta_PA = 0.0;
  double sin_delta_PA = 0.0;

  for (unsigned ibin=0; ibin<nbin; ibin++)
    if (linear0.get_amps()[ibin] > cutoff0 &&
	linear1.get_amps()[ibin] > cutoff1) {

      cos_delta_PA += q0[ibin]*q1[ibin] + u0[ibin]*u1[ibin];
      sin_delta_PA += q0[ibin]*u1[ibin] - q1[ibin]*u0[ibin];

    }

  double var_delta_PA = 0.0;

  for (unsigned ibin=0; ibin<nbin; ibin++)
    if (linear0.get_amps()[ibin] > cutoff0 &&
	linear1.get_amps()[ibin] > cutoff1) {

      double del = 0;

      // del delta_PA del q0
      del = cos_delta_PA * u1[ibin] - sin_delta_PA * q1[ibin];
      var_delta_PA += del * del * var_q0;

      // del delta_PA del u0
      del = -cos_delta_PA * q1[ibin] - sin_delta_PA * u1[ibin];
      var_delta_PA += del * del * var_u0;

      // del delta_PA del q1
      del = -cos_delta_PA * u0[ibin] - sin_delta_PA * q0[ibin];
      var_delta_PA += del * del * var_q1;

      // del delta_PA del u1
      del = cos_delta_PA * q0[ibin] - sin_delta_PA * u0[ibin];
      var_delta_PA += del * del * var_u1;

    }

  Estimate<double> radians( atan2(sin_delta_PA,cos_delta_PA), var_delta_PA );

  // return the answer in P.A. = 1/2 radians
  return 0.5 * radians;

}
