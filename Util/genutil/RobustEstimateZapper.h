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

// #define _DEBUG 1

#include "TextInterface.h"
#include "Estimate.h"

#include <algorithm>

//! Excises outliers from a container using robust statistics
class RobustEstimateZapper : public Reference::Able
{
private:
  bool error;
  bool logarithmic;
  bool scale;
  float threshold_min;
  float threshold_max;

public:

  RobustEstimateZapper ();

  //! Flag outliers based on their error bar
  bool get_error_bar () const { return error; }
  void set_error_bar (bool flag) { error = flag; }

  //! Flag outliers based on the logarithm of either the value or the error bar
  bool get_log () const { return logarithmic; }
  void set_log (bool flag) { logarithmic = flag; }

  //! Do not multiply threshold by MADM
  bool get_scale () const { return scale; }
  void set_scale (bool flag) { scale = flag; }

  //! Set threshold used to identify outliers (multiple of madm)
  float get_cutoff_threshold () const { return threshold_max; }
  void set_cutoff_threshold (float val) { threshold_max = threshold_min = val; }

  //! Set threshold used to identify outliers (multiple of madm)
  float get_cutoff_threshold_min () const { return threshold_min; }
  void set_cutoff_threshold_min (float val) { threshold_min = val; }

  //! Set threshold used to identify outliers (multiple of madm)
  float get_cutoff_threshold_max () const { return threshold_max; }
  void set_cutoff_threshold_max (float val) { threshold_max = val; }

  //! Return a text interface that can be used to access this instance
  TextInterface::Parser* get_interface();

  class Interface;

  float get_value (const Estimate<float>& datum)
  {
    float val;

    if (error)
      val = sqrt(datum.var);
    else
      val = datum.val;

    if (logarithmic)
      val = log10 (val);

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

#if _DEBUG
      std::cout << iparam << " " << ipt << " " << work[iwork] << std::endl;
#endif

      iwork ++;
    }

    // find the median value
    std::nth_element (work.begin(), work.begin()+iwork/2, work.begin()+iwork);
    float median = work[ iwork/2 ];

    float madm = 1.0;

    if (scale)
    {
      // compute the absolute deviation from the median
      for (unsigned ipt=0; ipt < iwork; ipt++)
        work[ipt] = fabs(work[ipt] - median);

      // find the median absolute deviation from the median 
      std::nth_element (work.begin(), work.begin()+iwork/2, work.begin()+iwork);
      madm = work[ iwork/2 ];
    }

#if _DEBUG
    std::cerr << "RobustEstimateZapper::excise iwork=" << iwork 
              << " iparam=" << iparam << " median=" << median
              << " madm=" << madm << std::endl;
#endif

    unsigned excised = 0;

    double max_extreme = median + threshold_max * madm;
    double min_extreme = median - threshold_min * madm;

    for (unsigned ipt=0; ipt<npt; ipt++)
    {
      Estimate<float> val = (container->*get)(iparam, ipt);
      if (val.var == 0.0)
        continue;

      double test = get_value(val);

      if (threshold_max && test > max_extreme)
      {
#if _DEBUG
    std::cerr << "RobustEstimateZapper::excise zapping >max iparam=" << iparam
              << " ipt=" << ipt << std::endl;
#endif

        excised ++;
        (container->*valid)(ipt, false);
      }

      if (threshold_min && test < min_extreme)
      {
#if _DEBUG
    std::cerr << "RobustEstimateZapper::excise zapping <min iparam=" << iparam
              << " ipt=" << ipt << std::endl;
#endif

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
