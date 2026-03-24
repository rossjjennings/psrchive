//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/stat/statutil.h

#ifndef __psrchive_Util_stat_statutil_h
#define __psrchive_Util_stat_statutil_h

#include <algorithm>
#include <vector>
#include "Error.h"

//! worker function for variance, skewness, kurtosis, etc.
void central_moments (std::vector<double> data, std::vector<double>& mu);

//! worker function for outlier detection
/*! pass a pointer to vector<float> as the second argument
  to retain the power spectral density used in this calculation */
double robust_variance (const std::vector<double>& data, std::vector<float>* psd = 0);

//! worker function for robust_variance and others
double median_upper_harmonic (const std::vector<double>& data, std::vector<float>* psd = 0);

//! worker function for robust_variance and others
void power_spectral_density (const std::vector<double>& data, std::vector<float>& psd);

template<typename T>
T median (std::vector<T> data)
{
  if (data.size() == 0)
    throw Error (InvalidParam, "median (statutil.h)", "input data size = 0");

  unsigned mid = data.size() / 2;
  std::nth_element( data.begin(), data.begin()+mid, data.end() );
  return data[mid];
}

template<typename T>
T mean (const std::vector<T>& data)
{
  if (data.size() == 0)
    throw Error (InvalidParam, "mean (statutil.h)", "input data size = 0");

  T sum (0.0);
  for (auto element: data)
    sum += element;
  return sum / data.size();
}

template<typename T>
void Q1_Q2_Q3 (std::vector<T> data, T& Q1, T& Q2, T& Q3)
{
  if (data.size() == 0)
    throw Error (InvalidParam, "Q1_Q2_Q3 (statutil.h)", "input data size = 0");

  std::sort( data.begin(), data.end() );
  unsigned ndat = data.size();
  Q1 = data[ndat / 4];
  Q2 = data[ndat / 2];
  Q3 = data[(3 * ndat) / 4];
}

template<typename T>
void filtered_Q1_Q2_Q3 (std::vector<T> data, T& Q1, T& Q2, T& Q3, T value)
{
  if (data.size() == 0)
    throw Error (InvalidParam, "filtered_Q1_Q2_Q3 (statutil.h)", "input data size = 0");
  
  std::remove( data.begin(), data.end(), value );
  std::sort( data.begin(), data.end() );
  unsigned ndat = data.size();
  Q1 = data[ndat / 4];
  Q2 = data[ndat / 2];
  Q3 = data[(3 * ndat) / 4];
}

#endif // __psrchive_Util_stat_statutil_h

