//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnProfileFitAnalysis.h,v $
   $Revision: 1.12 $
   $Date: 2006/08/21 12:36:44 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfileFitAnalysis_h
#define __Pulsar_PolnProfileFitAnalysis_h

#include "Pulsar/PolnProfileFit.h"
#include "MEAL/Complex2.h"
#include "Jones.h"

#include <vector>

namespace Pulsar {

  //! Analysis of the matrix template matching algorithm
  class PolnProfileFitAnalysis {

  public:

    //! Default constructor
    PolnProfileFitAnalysis ();

    //! When set, estimate the uncertainty in each attribute
    void set_compute_error (bool flag = true);

    //! Set the PolnProfileFit algorithm to be analysed
    void set_fit (PolnProfileFit*);

    //! Set the transformation to be used to find the optimal basis
    void set_basis (MEAL::Complex2* basis);

    //! Get the relative arrival time error
    Estimate<double> get_relative_error () const;

    //! Get the multiple correlation between phase shift and Jones parameters
    Estimate<double> get_multiple_correlation () const;

    //! Get the relative conditional arrival time error
    Estimate<double> get_relative_conditional_error () const;

  protected:

    //! The relative arrival time error
    Estimate<double> relative_error;

    //! The relative conditional arrival time error
    Estimate<double> relative_conditional_error;

    //! The multiple correlation between phase shift and Jones parameters
    Estimate<double> multiple_correlation;

    //! The PolnProfileFit algorithm to be analysed
    Reference::To<PolnProfileFit> fit;

    //! The transformation to be used to find the optimal basis
    Reference::To<MEAL::Complex2> basis;

    //! The partial derivative of the model gradient wrt K
    Jones<double> delgradient_delK (unsigned i, const Jones<double>& K) const;

    //! The partial derivative of the multiple correlation squared wrt S_k
    double delR2_varphiJ_delS (Matrix<8,8,double>& delC_delS);

    //! The partial derivative of the covariance matrix wrt Re[S_k] and Im[S_k]
    void delC_delS( Matrix<8,8,double>& delC_delSre,
		    Matrix<8,8,double>& delC_delSim, unsigned k ) const;

    //! The partial derivative of rho wrt basis parameter
    Jones<double> delrho_delB (unsigned b) const;

    //! The partial derivative of noise wrt basis parameter
    Stokes<float> delnoise_delB (unsigned b) const;

    //! The partial derivative of the curvature matrix wrt basis parameter
    Matrix<8,8,double> delalpha_delB (unsigned b) const;

    //! Get the curvature matrix
    void get_curvature (Matrix<8,8,double>& curvature);

    //! Set the model up to evaluate the specified harmonic
    void set_harmonic (unsigned index);

    Jones<double> model_result;
    std::vector< Jones<double> > model_gradient;

    Jones<double> xform_result;
    std::vector< Jones<double> > xform_gradient;

    Jones<double> phase_result;
    std::vector< Jones<double> > phase_gradient;

    Jones<double> basis_result;
    std::vector< Jones<double> > basis_gradient;

    //! the covariance matrix
    Matrix<8,8,double> covariance;

    //! the covariances between the Jones matrix parameters
    Matrix <7,7,double> C_JJ;

    //! the inverse of the covariances between the Jones matrix parameters
    Matrix <7,7,double> inv_C_JJ;

    //! the covariances between the phase shift and each Jones matrix parameter
    Vector <7,double> C_varphiJ;
    
    //! the variance of the unconditional phase shift
    double c_varphi;

    //! the multiple correlation squared
    double R2_varphiJ;

    double efac;

    //! Compute the uncertainty of results
    bool compute_error;

  };


  //! Analysis of the scalar template matching algorithm
  class ScalarProfileFitAnalysis {

  public:

    //! Set the PolnProfileFit algorithm to be analysed
    void set_fit (const PolnProfileFit*);

    //! Set the fluctuation spectrum of the Profile to be analyzed
    void set_spectrum (const Profile*);

    //! Set the maximum harmonic to be used from the fluctuation spectrum
    void set_max_harmonic (unsigned max_harmonic);

    //! Set the variance of the fluctuation spectrum
    void set_variance (double v);

    //! Get the variance of the fluctuation spectrum
    double get_variance () const { return variance; }

    //! Get the estimated phase shift error for the fluctuation spectrum
    Estimate<double> get_error () const;

    //! Get the curvature matrix
    void get_curvature (Matrix<2,2,double>& curvature);

    //! The partial derivative of the covariance matrix wrt Re[S_0] and Im[S_0]
    void delC_delS ( Matrix<2,2,double>& delC_delSre,
		     Matrix<2,2,double>& delC_delSim,
		     unsigned index ) const;

  protected:

    //! The PolnProfileFit algorithm to be analysed
    Reference::To<const Profile> spectrum;

    //! The amps in the fluctation spectrum
    const std::complex<float>* amps;

    //! The maximum harmonic
    unsigned max_harmonic;

    Matrix<2,2,double> covariance;
    double variance;
  };

}

#endif
