//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/TemplateUncertainty.h,v $
   $Revision: 1.10 $
   $Date: 2006/09/29 22:13:10 $
   $Author: straten $ */

#ifndef __Calibration_TemplateUncertainty_H
#define __Calibration_TemplateUncertainty_H

#include "Pulsar/ObservationUncertainty.h"
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
    virtual double get_weighted_norm (const Jones<double>&) const;
    
    //! Given a coherency matrix, return the weighted conjugate matrix
    virtual Jones<double> get_weighted_conjugate (const Jones<double>&) const;

  protected:

    //! The variance of the observed Stokes parameters
    Stokes<double> observation_variance;

    //! The variance of the template Stokes parameters
    Stokes<double> template_variance;

    //! The transformation from template to observation
    Reference::To<MEAL::Complex2> transformation;

    //! Flag set when inv_var attribute is up-to-date
    bool built;

    //! Callback method attached to transformation::changed signal
    void changed (MEAL::Function::Attribute a);

    //! Compute inv_var attribute
    virtual void build ();

  };

}

#endif
