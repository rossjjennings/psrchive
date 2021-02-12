/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SumThreshold.h"

#include <math.h>

#include <algorithm>
#include <numeric>
#include <utility>
#include <cassert>

Pulsar::SumThreshold::SumThreshold ()
{
  threshold = 4.0; // # sigma
  nlevel = 6; // number of flagging levels
}

// Run the algorithm in 1-D with nwin points.  initial mask input is updated on
// output.
void sumthreshold1(std::vector<float> &data, std::vector<float> &mask, 
    unsigned nwin, float cut)
{
  const unsigned ntot = data.size();
  std::vector<float> mask1(ntot,1.0);
  for (unsigned i=0; i<ntot-nwin+1; i++)
  {
    float sum = 0.0;
    for (unsigned j=0; j<nwin; j++)
      sum += mask[i+j]==0.0 ? cut : data[i+j];
    if (sum > ((float)nwin*cut)) 
      for (unsigned j=0; j<nwin; j++)
        mask1[i+j] = 0.0;
  }
  std::copy(mask1.begin(), mask1.end(), mask.begin());
}

unsigned Pulsar::SumThreshold::update_mask (std::vector<float> &mask, 
					    std::vector<float> &stat,
					    std::vector<float> &model,
					    unsigned nsubint,
					    unsigned nchan,
					    unsigned npol)
{
  const unsigned ntest = nsubint * nchan;
  std::vector<float> dattmp (ntest);
  std::vector<float> xn (npol);

  for (unsigned ipol=0; ipol < npol; ipol++)
  {
    unsigned valid = 0;

    for (unsigned isub=0; isub<nsubint; isub++)
    {
      for (unsigned ichan=0; ichan<nchan; ichan++)
      {
        unsigned idat = isub*nchan*npol + ichan*npol + ipol;
        unsigned imask = isub*nchan + ichan;

        if (mask[imask] == 0.0)
          continue;

        if (model[idat]!=0.0 && stat[idat]!=0.0 && mask[imask]!=0.0)
        {
          dattmp[valid] = stat[idat]/model[idat];
          valid++;
        }
      }
    }

    if (valid==0) return 0; // everything is masked

    assert (valid <= ntest);

    std::sort(dattmp.begin(), dattmp.begin() + valid);
    int qq = valid / 4;
    float rms = 1.35 * (dattmp[3*qq] - dattmp[qq]);

    // Cutoff for single-pixel
    xn[ipol] = threshold * rms;
  }

  unsigned total_masked = 0;
  
  for (unsigned ilev=0; ilev<nlevel; ilev++) 
  {
    // Number of points in this level
    const unsigned nwin = 1<<ilev;

    // Init next iteration of mask to 1.0
    std::vector<float> mask1(nsubint*nchan, 1.0);

    std::vector<float> dattmp, wttmp;

    // Do freq direction
    if (nwin<nchan)
    {
      dattmp.resize(nchan);
      wttmp.resize(nchan);
      for (unsigned isub=0; isub<nsubint; isub++)
      {
        for (unsigned ipol=0; ipol<npol; ipol++)
        {
          for (unsigned ichan=0; ichan<nchan; ichan++)
          {
            unsigned ii = isub*nchan*npol + ichan*npol + ipol;
            if (model[ii]!=0.0) 
            {
              dattmp[ichan] = stat[ii]/model[ii];
              wttmp[ichan] = mask[isub*nchan + ichan];
            }
            else
            {
              wttmp[ichan] = 0.0;
            }
          }
          sumthreshold1(dattmp,wttmp,nwin,1.0+xn[ipol]);
          for (unsigned ichan=0; ichan<nchan; ichan++)
            if (wttmp[ichan]==0.0)
              mask1[isub*nchan + ichan] = 0.0;
        }
      }
    }

    // do time direction
    if (nwin<nsubint)
    {
      dattmp.resize(nsubint);
      wttmp.resize(nsubint);
      for (unsigned ichan=0; ichan<nchan; ichan++)
      {
        for (unsigned ipol=0; ipol<npol; ipol++)
        {
          for (unsigned isub=0; isub<nsubint; isub++)
          {
            unsigned ii = isub*nchan*npol + ichan*npol + ipol;
            if (model[ii]!=0.0) 
            {
              dattmp[isub] = stat[ii]/model[ii];
              wttmp[isub] = mask[isub*nchan + ichan];
            }
            else
            {
              wttmp[isub] = 0.0;
            }
          }
          sumthreshold1(dattmp,wttmp,nwin,1.0+xn[ipol]);
          for (unsigned isub=0; isub<nsubint; isub++)
            if (wttmp[isub]==0.0)
              mask1[isub*nchan + ichan] = 0.0;
        }
      }
    }

    // Copy weights back to original array
    for (unsigned i=0; i<nsubint*nchan; i++)
      if (mask1[i]==0.0 && mask[i] != 0.0)
      {
	total_masked ++;
        mask[i] = 0.0;
      }

    // Reduce threshold for next level iteration
    for (unsigned ipol=0; ipol < npol; ipol++)
      xn[ipol] /= 1.5;
  }

  return total_masked;
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::SumThreshold::get_interface ()
{
  return new Interface (this);
}

Pulsar::SumThreshold::Interface::Interface (SumThreshold* instance)
{
  if (instance)
    set_instance (instance);

  add( &SumThreshold::get_threshold,
       &SumThreshold::set_threshold,
       "cutoff", "Outlier threshold [sigma]" );

  add( &SumThreshold::get_nlevel,
       &SumThreshold::set_nlevel,
       "nlevel", "Number of flagging levels" );
}

