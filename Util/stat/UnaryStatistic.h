//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/UnaryStatistic.h

#ifndef __UnaryStatistic_h
#define __UnaryStatistic_h

#include "Identifiable.h"
#include <algorithm>

//! Commmon statistics that can be derived from a single array of floats
class UnaryStatistic : public Identifiable
{
public:

  //! Create a new instance of UnaryStatistic based on name
  static UnaryStatistic* factory (const std::string& name);

  //! Returns a list of available UnaryStatistic children
  static const std::vector<UnaryStatistic*>& children ();

  //! Construct with a name and description
  UnaryStatistic (const std::string& name, const std::string& description);

  //! Derived types define the value returned
  virtual double get (const std::vector<double>&) = 0;

  //! Derived types must also define clone method
  virtual UnaryStatistic* clone () const = 0;

private:

  //! thread-safe build for factory
  static void build (); 
};

//! worker function for variance, skewness, kurtosis, etc.
void central_moments (std::vector<double> data, std::vector<double>& mu);

//! worker function for outlier detection
/*! pass a pointer to vector<float> as the second argument
  to retain the power spectral density used in this calculation */
double robust_variance (const std::vector<double>& data,
			std::vector<float>* psd = 0);

template<typename T>
T median (std::vector<T> data)
{
  unsigned mid = data.size() / 2;
  std::nth_element( data.begin(), data.begin()+mid, data.end() );
  return data[mid];
}

template<typename T>
T mean (const std::vector<T>& data)
{
  T sum (0.0);
  for (auto element: data)
    sum += element;
  return sum / data.size();
}

template<typename T>
void Q1_Q2_Q3 (std::vector<T> data, T& Q1, T& Q2, T& Q3)
{
  std::sort( data.begin(), data.end() );
  unsigned ndat = data.size();
  Q1 = data[ndat / 4];
  Q2 = data[ndat / 2];
  Q3 = data[(3 * ndat) / 4];
}

template<typename T>
void filtered_Q1_Q2_Q3 (std::vector<T> data, T& Q1, T& Q2, T& Q3, T value)
{
  std::remove( data.begin(), data.end(), value );
  std::sort( data.begin(), data.end() );
  unsigned ndat = data.size();
  Q1 = data[ndat / 4];
  Q2 = data[ndat / 2];
  Q3 = data[(3 * ndat) / 4];
}

#endif

