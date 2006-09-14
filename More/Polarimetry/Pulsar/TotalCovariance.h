//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/TotalCovariance.h,v $
   $Revision: 1.1 $
   $Date: 2006/09/14 15:18:36 $
   $Author: straten $ */

#ifndef __Calibration_TotalCovariance_H
#define __Calibration_TotalCovariance_H

#include "Calibration/TemplateUncertainty.h"
#include "Matrix.h"

namespace Calibration {

  //! Combines the uncertainty of the template and the observation
  class TotalCovariance : public TemplateUncertainty {

  public:

    //! Set the optimizing transformation
    void set_optimizing_transformation (const Matrix<4,4,double>& opt);

    //! Given a coherency matrix, return the weighted norm
    double get_weighted_norm (const Jones<double>&) const;
    
    //! Given a coherency matrix, return the weighted conjugate matrix
    Jones<double> get_weighted_conjugate (const Jones<double>&) const;

    //! Get the total variance in the specified Stokes parameter
    double get_variance (unsigned ipol) const;

  protected:

    //! The inverse of the total covariance matrix
    Matrix<4,4,double> inv_covar;

    //! The optimizing transformation
    Matrix<4,4,double> optimizer;

    //! Compute inv_covar attribute
    void build ();

  };

}

#endif
