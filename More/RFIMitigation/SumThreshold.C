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

void Pulsar::SumThreshold::update_mask (std::vector<float> &mask, 
    std::vector<float> &stat, std::vector<float> &model,
    unsigned nsubint, unsigned nchan, unsigned npol)
{
  const unsigned ntot = nsubint * nchan * npol;

  // Use IQR to get a robust measure of data/model RMS
  // Does this need to be done differently per poln?
  std::vector<float> dattmp;
  for (unsigned i=0; i<ntot; i++)
    if (model[i]!=0.0 && stat[i]!=0.0 && mask[i]!=0.0)
      dattmp.push_back(stat[i]/model[i]);
  if (dattmp.size()==0) return; // everything is masked
  std::sort(dattmp.begin(), dattmp.end());
  int qq = dattmp.size() / 4;
  const float rms = 1.35 * (dattmp[3*qq] - dattmp[qq]);

  // Cutoff for single-pixel
  const float x1 = threshold * rms;
  float xn = x1;

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
          sumthreshold1(dattmp,wttmp,nwin,1.0+xn);
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
          sumthreshold1(dattmp,wttmp,nwin,1.0+xn);
          for (unsigned isub=0; isub<nsubint; isub++)
            if (wttmp[isub]==0.0)
              mask1[isub*nchan + ichan] = 0.0;
        }
      }
    }

    // Copy weights back to original array
    for (unsigned i=0; i<nsubint*nchan; i++)
      if (mask1[i]==0.0) 
        mask[i] = 0.0;

    // Reduce threshold for next level iteration
    xn /= 1.5;
  }

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

