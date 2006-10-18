/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef _Util_genutil_templates_h
#define _Util_genutil_templates_h

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

//! Returns the maximum and minimum values on [i1, i2)
template <class T, class I>
void minmax (const I& it1, const I& it2, T& min, T& max, bool follow = false)
{
  if (!follow)
    max = min = *it1;

  for (I it=it1; it != it2; it++) {
    if (*it > max)
      max = *it;
    if (*it < min)
      min = *it;
  }
}

//! Returns the maximum and minimum values found in container c
template <class T, class C>
void minmaxval (const C& c, T& min, T& max, bool follow = false)
{
  minmax (c.begin(), c.end(), min, max, follow);
}

//! Returns true if element x is inside container c
template <class T, class C>
bool found (const T& x, const C& c)
{
  return std::find (c.begin(), c.end(), x) != c.end();
}

//! Returns the index of element x inside container c
template<class T, class C>
int index (const T& x, const C& c)
{
  typename C::const_iterator f = std::find (c.begin(), c.end(), x);
  if (f == c.end())
    return -1;

  return f - c.begin();
}

//! Removes the first instance of element x from container c
template<class T, class C>
bool remove (const T& x, C& c)
{
  typename C::iterator f = std::find (c.begin(), c.end(), x);
  if (f != c.end()) {
    c.erase(f);
    return true;
  }
  return false;
}

#endif
