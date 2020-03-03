//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/genutil/RobustEstimateZapper.h

#ifndef __RobustEstimateZapper_h
#define __RobustEstimateZapper_h

#include <iostream>

//! Excises outliers from a container using robust statistics
class RobustEstimateZapper
{
  public:
  bool error;
  bool logarithmic;
  float threshold;

  RobustEstimateZapper () { error = true; logarithmic = false; threshold = 3.0; }

  float get_value (const Estimate<float>& datum)
  {
    float val;

    if (error)
      val = sqrt(datum.var);
    else
      val = datum.val;

    if (logarithmic)
      val = log (val);

    return val;
  }

  template<class Container, class Size, class Get, class Valid>
  void excise (unsigned iparam, Container* container, Size size, Get get, Valid valid)
  {
    unsigned npt = (container->*size)();

#if _DEBUG
    std::cerr << "RobustEstimateZapper::excise iparam=" << iparam << " npt=" << npt << std::endl;
#endif

    std::vector<float> work (npt);
    unsigned iwork = 0;

    for (unsigned ipt=0; ipt<npt; ipt++)
    {
      Estimate<float> val = (container->*get)(iparam, ipt);
      if (val.var == 0.0)
        continue;

      work[iwork] = get_value (val);
      iwork ++;
    }

    // find the median value
    std::nth_element (work.begin(), work.begin()+iwork/2, work.begin()+iwork);
    float median = work[ iwork/2 ];

    // compute the absolute deviation from the median
    for (unsigned ipt=0; ipt < iwork; ipt++)
      work[ipt] = fabs(work[ipt] - median);

    // find the median absolute deviation from the median 
    std::nth_element (work.begin(), work.begin()+iwork/2, work.begin()+iwork);
    float madm = work[ iwork/2 ];

#if _DEBUG
    std::cerr << "RobustEstimateZapper::excise iwork=" << iwork << " median=" << median
              << " madm=" << madm << std::endl;
#endif

    unsigned excised = 0;

    for (unsigned ipt=0; ipt<npt; ipt++)
    {
      Estimate<float> val = (container->*get)(iparam, ipt);
      if (val.var == 0.0)
        continue;

      if (get_value(val) > median + threshold * madm)
      {
        excised ++;
        (container->*valid)(ipt, false);
      }
    }

#if _DEBUG
    std::cerr << "RobustEstimateZapper::excise excised=" << excised << std::endl;
#endif

  }
};

#endif
