/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <vector>
#include <assert.h>

template <class T>
void scrunch (std::vector<T>& vals, unsigned factor, bool mean = true)
{
  typename std::vector<T>::iterator into = vals.begin();
  typename std::vector<T>::iterator val;

  unsigned fi = 0;

  for (val = vals.begin(); val != vals.end(); into++) {
    *into = *val; val++;
    for (fi=1; fi<factor && val != vals.end(); (val++, fi++))
      *into += *val;
    if (mean)
      *into /= fi;
  }
  vals.resize (vals.size()/factor);
}

// returns the mean "bin" of a histogram
template <class T>
T histomean (const std::vector<T>& vals)
{
  T valcount = 0.0;
  T totcount = 0.0;

  T total = vals.size();
  T bin = 0.0;

  typename std::vector<T>::const_iterator val;

  for (val = vals.begin(); val != vals.end(); val++) {
    valcount += *val * bin;
    totcount += *val * total;

    bin += 1.0;
  }
  return valcount/totcount;
}

template <class T>
T sqr (const T& x)
{
  return x * x;
}

// return the sum of all elements in a vector
template <class T>
T sum (const std::vector<T>& x)
{
  T the_sum = 0.0;
  for (unsigned i=0; i<x.size(); i++)
    the_sum += x[i];
  return the_sum;
}

template <class T>
T mean (const std::vector<T>& x)
{
  return sum(x)/x.size();
}

template <class T>
T variance (const std::vector<T>& x)
{
  T the_mean = mean(x);
  T var = 0.0;
  
  for (unsigned i = 0; i < x.size(); i++)
    var += sqr(x[i] - the_mean);
  
  return var / T(x.size() - 1);
}

// normalize each element of a vector by the sum of all elements in it
template <class T>
void normalize (std::vector<T>& x)
{
  T the_sum = sum (x);
  assert( the_sum != 0 );
  for (unsigned i=0; i<x.size(); i++)
    x[i] /= the_sum;
}

template <class T>
void minmaxval (const std::vector<T>& vals, T& min, T& max, bool follow = false)
{
  if (!follow)
    max = min = vals[0];

  typename std::vector<T>::const_iterator val;
  for (val = vals.begin(); val != vals.end(); val++) {
    if (*val > max)
      max = *val;
    if (*val < min)
      min = *val;
  }
}

