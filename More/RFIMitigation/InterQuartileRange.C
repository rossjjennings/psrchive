/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/InterQuartileRange.h"

#include <algorithm>
#include <numeric>
#include <utility>
#include <cassert>

// #define _DEBUG 1

using namespace std;

unsigned get_minimum_slope_index (const std::vector<float>& data, float duty_cycle);

class IQRHelper
{
  std::vector<float> &mask;
	const std::vector<float> &stat;
	const std::vector<float> &model;

  public:

  IQRHelper (std::vector<float> &_mask, const std::vector<float> &_stat, const std::vector<float> &_model)
  : mask(_mask), stat(_stat), model(_model) {}

  unsigned nsubint = 0;
  unsigned nchan = 0;
  unsigned npol = 0;

  std::vector<float> data;
  unsigned valid;

  double Q1 = 0;
  double Q3 = 0;
  double IQR = 0;

  float cutoff_threshold_max = 1.5;
  float cutoff_threshold_min = 1.5;

  float minimum_slope_median_duty_cycle = 0.0;

  unsigned too_high = 0;
  unsigned too_low = 0;

  void set_ntest (unsigned ntest) 
  { data.resize(ntest); std::fill(data.begin(), data.end(), 0.0); valid = 0; }

  void add_data (unsigned isubint, unsigned ichan, unsigned ipol)
  {
    unsigned idat = isubint*nchan*npol + ichan*npol + ipol;
    unsigned imask = isubint*nchan + ichan;

    if (mask[imask] == 0.0)
      return;

    data[valid] = stat[idat];

    if (model.size() == stat.size() && model[idat] != 0.0)
      data[valid] -= model[idat];

    valid ++;
  }

  void compute ()
  {
    assert (valid <= data.size());
    data.resize(valid);
    std::sort (data.begin(), data.begin()+valid);

    unsigned offset = 0;
    
    if (minimum_slope_median_duty_cycle != 0.0)
    {
      unsigned msi = get_minimum_slope_index (data, minimum_slope_median_duty_cycle);
      unsigned medi = valid / 2;

      if (msi < medi)
      {
        DEBUG("msmi=" << msi << " less than medi=" << medi);
        valid = msi * 2;
      }
      else
      {
        DEBUG("msmi=" << msi << " greater than medi=" << medi);
        offset = (msi - medi) * 2;
        valid -= offset;
      }
    }

    unsigned iq1 = valid/4;
    unsigned iq3 = (valid*3)/4;

    DEBUG("iQ1=" << iq1 << " iQ3=" << iq3);

    Q1 = data[ iq1+offset ];
    Q3 = data[ iq3+offset ];
    IQR = Q3 - Q1;

#ifdef _DEBUG
    if (cutoff_threshold_min > 0)
      cerr << "MIN threshold=" << cutoff_threshold_min 
           << " Q1=" << Q1 << " IQR=" << IQR
           << " min=" << Q1 - cutoff_threshold_min * IQR << endl;
          
    if (cutoff_threshold_max > 0)
      cerr << "MAX threshold=" << cutoff_threshold_max
           << " Q3=" << Q3 << " IQR=" << IQR
           << " max=" << Q3 + cutoff_threshold_max * IQR << endl;
#endif
  }

  void test_data (unsigned isubint, unsigned ichan, unsigned ipol)
  {
    unsigned idat = isubint*nchan*npol + ichan*npol + ipol;
    unsigned imask = isubint*nchan + ichan;

    if (mask[imask] == 0.0)
      return;

    float value = stat[idat];

    if (model.size() == stat.size())
      value -= model[idat];

    bool zap = false;

    if (cutoff_threshold_min > 0 && value < Q1 - cutoff_threshold_min * IQR)
    {
      DEBUG("TOO LOW isubint=" << isub << " ichan=" << ichan << " ipol=" << ipol);
      zap = true;
      too_low ++;
    }

    if (cutoff_threshold_max > 0 && value > Q3 + cutoff_threshold_max * IQR)
    {
      DEBUG("TOO HIGH isubint=" << isub << " ichan=" << ichan << " ipol=" << ipol);

      zap = true;
      too_high ++;
    }

    if (zap)
      mask[imask] = 0;
  }
};

unsigned Pulsar::InterQuartileRange::update_mask
(
  std::vector<float> &mask, 
  std::vector<float> &stat,
  std::vector<float> &model,
  unsigned nsubint,
  unsigned nchan,
  unsigned npol)
{
  IQRHelper helper (mask, stat, model);
  helper.nsubint = nsubint;
  helper.nchan = nchan;
  helper.npol = npol;
  helper.cutoff_threshold_max = cutoff_threshold_max;
  helper.cutoff_threshold_min = cutoff_threshold_min;
  helper.minimum_slope_median_duty_cycle = minimum_slope_median_duty_cycle;

  // number of values tested per polarization

#if _DEBUG
  cerr << "InterQuartileRange::update_mask nsubint=" << nsubint
       << " nchan=" << nchan << " npol=" << npol << " ntest=" << ntest 
       << " stat.sz=" << stat.size() << " model.sz=" << model.size()
       << " mask.sz=" << mask.size() << endl;
#endif

  for (unsigned ipol=0; ipol < npol; ipol++)
  {
    if (way == "all")
    {
      helper.set_ntest(nsubint * nchan);

      for (unsigned isub=0; isub<nsubint; isub++)
      {
        for (unsigned ichan=0; ichan<nchan; ichan++)
        {
          helper.add_data(isub,ichan,ipol);
        }
      }

      helper.compute();

      for (unsigned isub=0; isub<nsubint; isub++)
      {
        for (unsigned ichan=0; ichan<nchan; ichan++)
        {
          helper.test_data (isub, ichan, ipol);
        }
      }
    }
    else if (way == "time")
    {
      for (unsigned ichan=0; ichan<nchan; ichan++)
      {
        helper.set_ntest(nsubint);

        for (unsigned isub=0; isub<nsubint; isub++)
        {
          helper.add_data(isub,ichan,ipol);
        }

        helper.compute();

        for (unsigned isub=0; isub<nsubint; isub++)
        {
          helper.test_data (isub, ichan, ipol);
        }
      }
    }
    else if (way == "freq")
    {
      for (unsigned isub=0; isub<nsubint; isub++)
      {
        helper.set_ntest(nchan);

        for (unsigned ichan=0; ichan<nchan; ichan++)
        {
          helper.add_data(isub,ichan,ipol);
        }

        helper.compute();

        for (unsigned ichan=0; ichan<nchan; ichan++)
        {
          helper.test_data (isub, ichan, ipol);
        }
      }
    }    
  }

  DEBUG("too high=" << helper.too_high << " too low=" << helper.too_low);

  return helper.too_high + helper.too_low;
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::InterQuartileRange::get_interface ()
{
  return new Interface (this);
}

Pulsar::InterQuartileRange::Interface::Interface (InterQuartileRange* instance)
{
  if (instance)
    set_instance (instance);

  add( &InterQuartileRange::get_cutoff_threshold_min,
       &InterQuartileRange::set_cutoff_threshold_min,
       "cutmin", "Outlier threshold: Q1-cutmin*IQR - Q3+cutmax*IQR" );

  add( &InterQuartileRange::get_cutoff_threshold_max,
       &InterQuartileRange::set_cutoff_threshold_max,
       "cutmax", "Outlier threshold: Q1-cutmin*IQR - Q3+cutmax*IQR" );

  add( &InterQuartileRange::get_minimum_slope_median_duty_cycle,
       &InterQuartileRange::set_minimum_slope_median_duty_cycle,
       "msm", "Duty cycle used to find the minimum slope median" );

  add(&InterQuartileRange::get_way,
      &InterQuartileRange::set_way,
      "way", "'time' or 'freq' or 'all'" );
}

double get_slope (const std::vector<float>& data, unsigned istart, unsigned iend)
{
  double xmean = 0.0;
  double ymean = 0.0;
  unsigned n = 0;
  for (unsigned i=istart; i<=iend; i++)
  {
    xmean += i;
    ymean += data[i];
    n++;
  }

  xmean /= n;
  ymean /= n;

  double covar = 0.0;
  double xvar = 0.0;
    
  for (unsigned i=istart; i<=iend; i++)
  {
    double dx = i - xmean;
    double dy = data[i] - ymean;
    covar += dx * dy;
    xvar += dx * dx;
  }

  return covar / xvar;
}

unsigned get_minimum_slope_index (const std::vector<float>& data, float duty_cycle)
{
  unsigned ndat = data.size();
  unsigned nslope = ndat * duty_cycle;
  unsigned half = nslope / 2;

  unsigned imin = ndat / 2;
  double min_slope = -1;
    
  for (unsigned idat=half; idat+half < ndat; idat++)
  {
    double slope = get_slope (data, idat-half, idat+half);
    if (slope < min_slope || min_slope == -1)
    {
      min_slope = slope;
      imin = idat;
    }
  }

  return imin;
}
