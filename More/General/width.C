/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
using namespace Pulsar;

float width (const Profile* profile, float& error, float pc, float dc)
{
  unsigned nbin = profile->get_nbin();
  
  float min_phs = profile->find_min_phase(dc);
  int   min_bin = int(min_phs * float(nbin));

  double min_mean = 0.0;
  double min_var  = 0.0;
  double var_var  = 0.0;

  int start = int(float(nbin) * (min_phs - dc/2.0));
  int end   = int(float(nbin) * (min_phs + dc/2.0));

  profile->stats(&min_mean, &min_var, &var_var, start, end);

  float stdev = sqrt(min_var);

  float level = (profile->max() - min_mean) * pc/100.0 - stdev + min_mean;

  std::vector<float> results;

  for (unsigned tries = 0; tries < 3; tries++)
  {
    if (level < 5.0*min_var)
    {
      return 0.0;
    }

    float lo_edge = 0.0;
    float hi_edge = 0.0;

    int bc = min_bin;

    while (1)
    {
      if (profile->get_amps()[bc] > level)
      {
        // Linearly interpolate between both sides of the edge
        float x1 = bc;
        float x2 = bc+1;

        float y1 = profile->get_amps()[bc];
        float y2 = profile->get_amps()[(bc+1)%nbin];

        float slope = (y2 - y1) / (x2 - x1);
	hi_edge = x1 + (level-y1)/slope;
        break;
      }
      bc--;
      if (bc < 0)
        bc = nbin - 1;
      if (bc == min_bin)
        break;
    }

    bc = min_bin;

    while (1)
    {
      if (profile->get_amps()[bc] > level)
      {
        // Linearly interpolate between both sides of the edge
        float x1 = bc-1;
        float x2 = bc;

        float y1 = profile->get_amps()[(bc+nbin-1)%nbin];
        float y2 = profile->get_amps()[bc];

        float slope = (y2 - y1) / (x2 - x1);
        lo_edge = x1 + (level-y1)/slope;
        break;
      }
      bc++;
      if (bc > int(nbin-1))
        bc = 0;
      if (bc == min_bin)
        break;
    }

    float bin_dist = hi_edge - lo_edge;
    if (bin_dist < 0)
      bin_dist += float(nbin);

    results.push_back(bin_dist / float(nbin));
    level += stdev;
  }

  error = fabs(results.front()-results.back())/2.0;
  return results[1];
}
