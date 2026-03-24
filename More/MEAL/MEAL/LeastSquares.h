//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/LeastSquares.h

#ifndef __MEAL_LeastSquares_H
#define __MEAL_LeastSquares_H

#include "Reference.h"

namespace MEAL {

  /*! General properties of least squares minimization */
  class LeastSquares : public Reference::Able
  {

  public:

    typedef std::vector< std::vector<double> > matrix;

    //! Return the name of the fit algorithm
    virtual std::string get_name () const = 0;

    //! Set the maximum number of iterations in fit algorithm
    void set_maximum_iterations (unsigned maximum_iterations);

    //! Get the maximum number of iterations in fit algorithm
    unsigned get_maximum_iterations () const { return maximum_iterations; }

    //! Set the convergence threshold as an absolute value of chisq
    void set_convergence_chisq (float chisq);

    //! Set the convergence threshold as relative change in parameters
    void set_convergence_delta (float delta_parameters);

    //! Set the reduced chi-squared above which the fit is considered bad
    void set_maximum_reduced_chisq (float maximum_reduced_chisq);

    //! Set the verbosity during solve
    void set_debug (bool flag = true) { debug = flag; }

    //! Set the solved flag (can be used to flag bad data)
    void set_solved (bool val) { solved = val; }

    //! Return true when the fit has been solved
    bool get_solved () const { return solved; }

    //! Set the singular flag (can be used to flag bad data)
    void set_singular (bool val) { singular = val; }

    //! Return true when the fit has failed due to singularity
    bool get_singular () const { return singular; }

    //! The number of iterations in last call to solve method
    unsigned get_iterations () const { return iterations; }

    //! The chi-squared in last call to solve method
    float get_chisq () const { return best_chisq; }

    //! The number of degrees of freedom in last call to solve method
    unsigned get_nfree () const { return nfree; }

    //! Get the number of parameters in fit
    unsigned get_nparam_infit () const { return nparam_infit; }

    //! The total number of constraints (one-dimensional data)
    unsigned get_ndat_constraint () const { return ndat_constraint; }

    //! The logarithm of the determinant of the chi-squared curvature matrix
    double get_log_det_curvature () const { return log_det_Hessian; }

    //! The logarithm of the condition number of the chi-squared curvature matrix
    double get_log_cond_curvature () const { return log_cond_Hessian; }

    //! Get the covariance matrix of the last fit
    void get_covariance (matrix& c) const { c = covariance; }

  protected:

    //! The maximum number of iterations in during fit
    unsigned maximum_iterations = 50;

    //! The convergence chisq
    float convergence_chisq = 0.0;

    //! The convergence delta
    float convergence_delta = 0.01;

    //! The maximum reduced chi-squared allowed
    float maximum_reduced = 0.0;

    //! The number of iterations in last call to solve method
    unsigned iterations = 0;

    //! Logarithm of the determinant of the Hessian matrix
    double log_det_Hessian = 0.0;

    //! Logarithm of the condition number of the Hessian matrix
    double log_cond_Hessian = 0.0;

    //! The best chi-squared in last call to solve method
    float best_chisq = 0.0;

    //! The number of free parameters in last call to solve method
    unsigned nfree = 0;

    //! The number of parameters to be fit
    unsigned nparam_infit = 0;

    //! The total number of constraints (multi-dimensional data)
    unsigned ndat_constraint = 0;

    //! The fit debug mode
    bool debug = false;

    //! Set true when fit has been solved
    bool solved = false;

    //! Set true when the fit failed due to a singular Hessian matrix
    bool singular = false;

    //! The covariance matrix set after fitting
    matrix covariance;
  };
}

#endif

