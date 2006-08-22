//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/StokesError.h,v $
   $Revision: 1.1 $
   $Date: 2006/08/22 22:00:20 $
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

  protected:

    //! The Jones matrix of the transformation
    Jones<MEAL::ScalarValue> xform;

    //! The input Stokes parameter variances
    Stokes<MEAL::ScalarMath> input;

    //! The output Stokes parameter variances
    Stokes<MEAL::ScalarMath> output;

  };

}

#endif
