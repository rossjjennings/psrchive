//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/StokesError.h,v $
   $Revision: 1.2 $
   $Date: 2006/08/23 15:29:42 $
   $Author: straten $ */

#ifndef __MEAL_StokesError_H
#define __MEAL_StokesError_H

#include "Stokes.h"
#include "Jones.h"

#include "MEAL/ScalarValue.h"
#include "MEAL/ScalarMath.h"
#include "MEAL/Complex2.h"

namespace MEAL {

  //! Propagates Stokes parameter uncertainty through congruence transformation
  class StokesError : public Reference::Able {

  public:

    //! Default constructor
    StokesError ();

    //! Set the variances of the input Stokes parameters
    void set_variance (const Stokes<double>&);

    //! Set the transformation
    void set_transformation (const Jones<double>&);

    //! Get the variances of the output Stokes parameters
    void get_variance (Stokes<double>&);


    //! Set the transformation gradient component
    void set_transformation_gradient (const Jones<double>&);

    //! Get the variances of the output Stokes parameters gradient component
    void get_variance_gradient (Stokes<double>&);

  protected:

    //! The input Stokes parameter variances
    Stokes<MEAL::ScalarMath> input;

    //! The Jones matrix of the transformation
    Jones<MEAL::ScalarValue> xform;

    //! The output Stokes parameter variances
    Stokes<MEAL::ScalarMath> output;

    //! The gradient of Stokes parameter variances is built only when needed
    bool gradient_built;

    void build_gradient ();

    //! The Jones matrix of the transformation gradient component
    Jones<MEAL::ScalarValue> xform_grad;

    //! The output Stokes parameter variances gradient component
    Stokes<MEAL::ScalarMath> output_grad;


  };

}

#endif
