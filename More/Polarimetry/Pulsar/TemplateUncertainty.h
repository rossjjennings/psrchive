//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/TemplateUncertainty.h,v $
   $Revision: 1.7 $
   $Date: 2006/09/14 14:07:32 $
   $Author: straten $ */

#ifndef __Calibration_TemplateUncertainty_H
#define __Calibration_TemplateUncertainty_H

#include "Calibration/ObservationUncertainty.h"

#include "MEAL/StokesError.h"
#include "MEAL/Complex2.h"

namespace Calibration {

  //! Combines the uncertainty of the template and the observation
  class TemplateUncertainty : public ObservationUncertainty {

  public:

    //! Default constructor
    TemplateUncertainty ();

    //! Set the uncertainty of the observation
    void set_variance (const Stokes<double>& var);

    //! Set the uncertainty of the template
    void set_template_variance (const Stokes<double>& var);

    //! Set the transformation from template to observation
    void set_transformation (const MEAL::Complex2* transformation);

    //! Given a coherency matrix, return the weighted norm
    double get_weighted_norm (const Jones<double>&) const;
    
    //! Given a coherency matrix, return the weighted conjugate matrix
    Jones<double> get_weighted_conjugate (const Jones<double>&) const;

    //! Get the Stokes uncertainty estimator
    MEAL::StokesError* get_StokesError () { return &template_variance; }

    //! Get the total variance in the specified Stokes parameter
    double get_variance (unsigned ipol) const;

  protected:

    //! The variance of the observed Stokes parameters
    Stokes<double> observation_variance;

    //! The variance of the template Stokes parameters
    MEAL::StokesError template_variance;

    //! The transformation from template to observation
    Reference::To<MEAL::Complex2> transformation;

    //! Flag set when inv_var attribute is up-to-date
    bool built;

    //! Callback method attached to transformation::changed signal
    void changed (MEAL::Function::Attribute a);

    //! Compute inv_var attribute
    void build ();

  };

}

#endif
