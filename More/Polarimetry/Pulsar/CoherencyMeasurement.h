//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CoherencyMeasurement.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 16:41:06 $
   $Author: straten $ */

#ifndef __Calibration_CoherencyMeasurement_H
#define __Calibration_CoherencyMeasurement_H

#include "Estimate.h"
#include "Stokes.h"
#include "Jones.h"

#include <complex>

namespace Calibration {

  //! A coherency matrix measurement and its estimated error
  class CoherencyMeasurement {

  public:

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

  };

}

#endif
