/***************************************************************************
 *
 *   Copyright (C) 2000 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <vector>
#include <assert.h>

template <class T>
void scrunch (std::vector<T>& vals, unsigned factor, bool integrate = true)
{
  typename std::vector<T>::iterator into = vals.begin();
  typename std::vector<T>::iterator val;

  for (val = vals.begin(); val != vals.end(); into++) {
    *into = *val; val++;
    for (unsigned fi=1; fi<factor && val != vals.end(); (val++, fi++))
      *into += *val;
  }
  vals.resize (vals.size()/factor);
}

// returns the mean "bin" of a histogram
template <class T>
double histomean (const std::vector<T>& vals)
{
  double valcount = 0.0;
  double totcount = 0.0;

  double total = (double) vals.size();
  double bin = 0.0;

  typename std::vector<T>::iterator val;

  for (val = vals.begin(); val != vals.end(); val++) {
    valcount += *val * bin;
    totcount += *val * total;

    bin += 1.0;
  }
  return valcount/totcount;
}

// return the sum of all elements in a vector
template <class T>
double sum (const std::vector<T>& x)
{
  double the_sum = 0.0;
  for (unsigned i=0; i<x.size(); i++)
    the_sum += x[i];
  return the_sum;
}

// normalize each element of a vector by the sum of all elements in it
template <class T>
void normalize (std::vector<T>& x)
{
  double the_sum = sum (x);
  assert( the_sum != 0 );
  for (unsigned i=0; i<x.size(); i++)
    x[i] /= the_sum;
}
