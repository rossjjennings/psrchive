//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/EstimateStats.h

#ifndef __EstimateStats_h
#define __EstimateStats_h

#include "Estimate.h"
#include <vector>
#include <algorithm>

template <typename T, typename U>
double chisq (const Estimate<T,U>& a, const Estimate<T,U>& b) 
{
  T diff = a.get_value() - b.get_value();
  T var  = a.get_variance() + b.get_variance();

  if (var == 0.0)
    return 0;
  else
    return diff * diff / var;
}

template <typename T, typename U>
double chisq (const MeanEstimate<T,U>& a, const MeanEstimate<T,U>& b) 
{
  return chisq (a.get_Estimate(), b.get_Estimate());
}

template <typename T, typename U>
double chisq (const MeanRadian<T,U>& a, const MeanRadian<T,U>& b)
{
  return 0.5 * ( chisq(a.get_cos(), b.get_cos()) + 
		 chisq(a.get_sin(), b.get_sin()) );
}

template <typename T, typename U>
Estimate<T,U> weighted_mean (const std::vector< Estimate<T,U> >& vals)
{
  MeanEstimate<T,U> mean;
  for (auto element: vals)
    mean += element;
  return mean.get_Estimate();
}

template <typename T, typename U>
void weighted_quartiles (const std::vector< Estimate<T,U> >& vals,
			 Estimate<T,U>& Q1,
			 Estimate<T,U>& Q2,
			 Estimate<T,U>& Q3)
{
  unsigned nval = vals.size();
  
  if (vals.size () < 3)
    throw Error (InvalidState, "weighted_quartiles",
		 "ndat=%u < 3", nval);

  // remove all esimates with zero variance (flagged as bad)
  std::vector< Estimate<T,U> > cleaned ( nval );
  unsigned iclean = 0;
  for (unsigned ival=0; ival < nval; ival++)
    if (vals[ival].var > 0)
      {
	cleaned[iclean] = vals[ival];
	iclean ++;
      }

#if _DEBUG
  std::cerr << "weighted_quartiles: nval=" << nval << " nclean=" << iclean << std::endl;
#endif
  
  nval = iclean;
  
  cleaned.resize (nval);
  
  U total_weight = 0.0;
  for (auto element: cleaned)
    total_weight += 1.0 / element.var;

  std::sort (cleaned.begin(), cleaned.end());

  U quartile_weight[3];
  for (unsigned i=0; i<3; i++)
    quartile_weight[i] = 0.25 * (i+1) * total_weight;

  unsigned quartile_index[3] = { 0, 0, 0 };
  
  U weight = 0.0;

  for (unsigned ival=0; ival < nval; ival++)
  {
    weight += 1.0 / cleaned[ival].var;

    for (unsigned i=0; i<3; i++)
      if (weight < quartile_weight[i])
	quartile_index[i] = ival;
  }

#if _DEBUG
  std::cerr << "weighted_quartile indeces: ";
  for (unsigned i=0; i<3; i++)
    std::cerr << " Q" << i+1 << "=" << quartile_index[i];
  std::cerr << std::endl;
#endif
  
  Q1 = Estimate<T,U> (cleaned[quartile_index[0]].val, 1.0/total_weight);
  Q2 = Estimate<T,U> (cleaned[quartile_index[1]].val, 1.0/total_weight);
  Q3 = Estimate<T,U> (cleaned[quartile_index[2]].val, 1.0/total_weight);
}

template <typename T, typename U>
Estimate<T,U> weighted_median (const std::vector< Estimate<T,U> >& vals)
{
  Estimate<T,U> Q1, Q2, Q3;
  weighted_quartiles (vals, Q1, Q2, Q3);
  return Q2;
}

#endif
