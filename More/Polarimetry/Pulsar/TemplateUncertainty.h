//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/TemplateUncertainty.h,v $
   $Revision: 1.4 $
   $Date: 2006/08/22 22:05:38 $
   $Author: straten $ */

#ifndef __Calibration_TemplateUncertainty_H
#define __Calibration_TemplateUncertainty_H

#include "Calibration/CoherencyMeasurement.h"

#include "MEAL/ScalarValue.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/Complex2.h"

namespace Calibration {

  //! Combines the uncertainty of the template and the observation
  class TemplateUncertainty : public CoherencyMeasurement::Uncertainty {

  public:

    //! Default constructor
    TemplateUncertainty ();

    //! Return the inverse of the variance of the specified polarization
    double get_inv_var (unsigned ipol) const;
    
    //! Set the uncertainty of the observation
    void set_observation_var (const Stokes<double>& var);

    //! Set the uncertainty of the template
    void set_template_var (const Stokes<double>& var);

    //! Set the transformation from template to observation
    void set_transformation (const MEAL::Complex2* transformation);

  protected:

    //! The variance of the observed Stokes parameters
    Stokes<double> observation_variance;

    //! The variance of the template Stokes parameters
    Stokes< Estimate<double> > template_variance;

    //! The transformation from template to observation
    Reference::To<MEAL::Complex2> transformation;

    //! The Jones matrix of the transformation
    Jones<MEAL::ScalarValue> xform;

    //! The input Stokes parameter variances
    Stokes<MEAL::ScalarMath> input;

    //! The output Stokes parameter variances
    Stokes<MEAL::ScalarMath> output;

    //! The inverse of the combined variance
    Stokes<double> inv_var;

    //! Flag set when inv_var attribute is up-to-date
    bool built;

    //! Callback method attached to transformation::changed signal
    void changed (MEAL::Function::Attribute a);

    //! Compute inv_var attribute
    void build ();

  };

}

#endif
