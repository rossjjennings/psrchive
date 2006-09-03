//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/TemplateUncertainty.h,v $
   $Revision: 1.6 $
   $Date: 2006/09/03 00:09:28 $
   $Author: straten $ */

#ifndef __Calibration_TemplateUncertainty_H
#define __Calibration_TemplateUncertainty_H

#include "Calibration/CoherencyMeasurement.h"

#include "MEAL/StokesError.h"
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

    //! Get the sum of the template and observation variance
    Stokes<double> get_input_var () const;

    //! Set the transformation from template to observation
    void set_transformation (const MEAL::Complex2* transformation);

    //! Get the Stokes uncertainty estimator
    MEAL::StokesError* get_StokesError () { return &template_variance; }

  protected:

    //! The variance of the observed Stokes parameters
    Stokes<double> observation_variance;

    //! The variance of the template Stokes parameters
    MEAL::StokesError template_variance;

    //! The transformation from template to observation
    Reference::To<MEAL::Complex2> transformation;

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
