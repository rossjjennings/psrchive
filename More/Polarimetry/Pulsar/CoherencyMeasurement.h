//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CoherencyMeasurement.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/17 13:34:44 $
   $Author: straten $ */

#ifndef __Calibration_CoherencyMeasurement_H
#define __Calibration_CoherencyMeasurement_H

#include "Reference.h"
#include "Estimate.h"
#include "Stokes.h"
#include "Jones.h"

#include <complex>

namespace Calibration {

  //! A coherency matrix measurement and its estimated error
  /* Among other things, this class implements the interface of the
     MEAL::WeightingScheme class used by MEAL::LevenbergMacquardt */
  class CoherencyMeasurement {

  public:

    //! Estimates the uncertainty of a CoherencyMeasurement
    class Uncertainty : public Reference::Able {
    public:
      //! Return the inverse of the variance of the specified polarization
      virtual float get_inv_var (unsigned ipol) const = 0;
    };

    //! Default constructor
    CoherencyMeasurement (unsigned input_index = 0);

    //! Set the index of the input to which the measurement corresponds
    void set_input_index (unsigned index);

    //! Set the index of the input to which the measurement corresponds
    unsigned get_input_index () const;

    //! Set the measured Stokes parameters
    void set_stokes (const Stokes< Estimate<double> >& stokes);

    //! Set the measured complex Stokes parameters
    void set_stokes (const Stokes< std::complex<double> >& stokes,
		     const Stokes<double>& variance);

    //! Set the measured complex Stokes parameters and the variance function
    void set_stokes (const Stokes< std::complex<double> >& stokes, 
		     const Uncertainty* var);

    //! Get the measured coherency matrix
    Jones<double> get_coherency () const;

    //! Get the measured Stokes parameters
    Stokes< Estimate<double> > get_stokes () const;

    //! Get the variance of the specified polarization
    float get_variance (unsigned ipol) const;

    //! Given a coherency matrix, return the weighted norm
    double get_weighted_norm (const Jones<double>& matrix) const;

    //! Given a coherency matrix, return the weighted conjugate matrix
    Jones<double> get_weighted_conjugate (const Jones<double>& matrix) const;

  protected:

    //! Index of the input to which the measurement corresponds
    unsigned input_index;

    //! The coherency matrix measurement
    Jones<double> rho;

    //! The inverse of the variance in each Stokes parameter
    Stokes<float> inv_var;

    //! The uncertainty of the measurement
    const Uncertainty* uncertainty;

    //! Get the inverse of the variance of the specified polarization
    float get_inv_var (unsigned ipol) const {
      if (uncertainty)
	return uncertainty->get_inv_var(ipol);
      return inv_var[ipol];
    }

  };

}

#endif
