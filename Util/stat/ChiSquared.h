//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 - 2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/stat/ChiSquared.h

#ifndef __Util_stat_ChiSquared_h
#define __Util_stat_ChiSquared_h

#include "BinaryStatistic.h"

namespace BinaryStatistics
{
  class ChiSquared : public BinaryStatistic
  {
    bool robust_linear_fit = true;
    double outlier_threshold = 3.0;
    double max_zap_fraction = 0.5;

    std::vector<double> residual;
    
  public:

    //! Default constructor
    ChiSquared ();

    //! Set the threshold used to detect outliers during robust linear fit
    void set_outlier_threshold (double threshold) { outlier_threshold = threshold; }
    
    //! Return the chi-squared difference between A and B
    /*! If robust_linear_fit is true, then B is scaled and offset to minimize the chi-squared difference. */
    double get (const std::vector<double>& A, const std::vector<double>& B);

    //! Return the residual = A - scale*B - offset, where scale and offset are determined by the robust linear fit
    const std::vector<double>& get_residual () const { return residual; }
    
    ChiSquared* clone () const { return new ChiSquared; }
  };
}

#endif
