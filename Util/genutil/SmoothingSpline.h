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

  unsigned ndat_good;
  
public:

  //! Default constructor
  SmoothingSpline ();
  
  //! Set the smoothing factor
  void set_smoothing (double);

  //! Set the effective number of freely estimated parameters
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

  //! Return the number of good data points included in the fit
  unsigned get_ndat_good () const;
  
  double get_fit_gcv ();
  double get_fit_msre ();
  double get_fit_effective_nfree ();
  double get_fit_smoothing ();
  double get_fit_true_mse ();
  double get_fit_Gauss_Markov_error_variance ();

  //! evaluate the spline at the specified argument
  double evaluate (double);
};

//! Determines the spline smoothing factor as in Clark (1977)
/*! For small numbers of data points to be fit and at low
  signal-to-noise ratios, the GCV function exhibits multiple local
  minima and in practice, for around 10% to 20% of trials, the GCVSPL
  sub-routine yields smoothing splines that overfit the data.

  The m-fold cross-validation technique described in Section 4 of
  Clark (1977) overcomes this issue.

  R. M. Clark, Non-Parametric Estimation of a Smooth Regression
  Function, Journal of the Royal Statistical Society. Series B
  (Methodological), 1977, Vol. 39, No. 1 (1977), pp. 107-113
  https://www.jstor.org/stable/2984885
*/

class CrossValidatedSmoothing
{
  bool logarithmic;             // linearly space smoothing factors on logarithmic scale
  unsigned ntrial;              // number of trial smoothing factors
  unsigned npartition;          // m=40 in Clark (1977)
  double validation_fraction;   // 0.1 in Clark (1977)
  SmoothingSpline* spline;      // the spline implementation
  
public:

  CrossValidatedSmoothing ();

  void set_spline (SmoothingSpline* _spline) { spline = _spline; }
  
  //! Fit spline to data using current configuration
  void fit ( std::vector< double >& data_x,
	     std::vector< Estimate<double> >& data_y);

  //! Return the mean goodness-of-fit for the current smoothing
  double get_mean_gof (const std::vector< double >& data_x,
		       const std::vector< Estimate<double> >& data_y);

  //! Get the trial smoothing factors
  void get_nfree_trials (std::vector<double>& nfree, unsigned ndat);

};

class BootstrapUncertainty
{
  unsigned nsample;
  SmoothingSpline* spline;      // the spline implementation
  
public:

  BootstrapUncertainty ();

  void set_spline (SmoothingSpline* _spline) { spline = _spline; }
  
  void get_uncertainty (const std::vector< double >& dat_x,
			std::vector< Estimate<double> >& dat_y);
};
  
#endif
