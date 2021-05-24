//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/Util/units/SmoothingSpline.h

#ifndef __SmoothingSpline_h
#define __SmoothingSpline_h

#include "Estimate.h"
#include <vector>

//! Interface to GCVSPL sub-routine by Herman J. Woltring
/*! Reference:
    H.J. Woltring (1986), A Fortran package for generalized, 
    cross-validatory spline smoothing and differentiation.
    Adv. Eng. Software 8(3), 104-113
*/
class SmoothingSpline
{
  int current_knot;
  std::vector<double> knot;
  std::vector<double> coefficients;
  std::vector<double> workspace;
  
  double smoothing;
  double effective_nfree;
  double chi_squared;
  bool minimize_gcv;

public:

  //! Default constructor
  SmoothingSpline ();
  
  //! Set the smoothing factor
  void set_smoothing (double);

  //! Set the effective number of freely esimated parameters
  void set_effective_nfree (double);

  //! Set the mean squared residual error, MSRE (Equation 5)
  /*! Each term in equation 5 is multiplied by a weight, which is usually
    the inverse of the variance.  In this case, Equation 5 is equivalent to the
    reduced chi-squared.  

    When the MSRE is specified, the smoothing factor is determined by 
    minimizing the true predicted mean-squared error (Equation 6) */
  void set_msre (double);

  //! Determine the smoothing factor using generalized cross-validation
  void set_minimize_gcv (bool);
  
  //! Fit spline to data using current configuration
  void fit (const std::vector< double >& data_x,
	    const std::vector< Estimate<double> >& data_y);

  double get_fit_gcv ();
  double get_fit_msre ();
  double get_fit_effective_nfree ();
  double get_fit_smoothing ();
  double get_fit_true_mse ();
  double get_fit_Gauss_Markov_error_variance ();

  //! evaluate the spline at the specified argument
  double evaluate (double);
};

#endif
