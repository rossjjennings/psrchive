//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Invariant.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/17 13:35:25 $
   $Author: straten $ */

#ifndef __MEAL_Invariant_h
#define __MEAL_Invariant_h

#include "Stokes.h"
#include "Estimate.h"
#include "ScalarMath.h"

namespace MEAL {

  class Scalar;

  //! Computes the invariant interval
  /*! This class properly handles the propagation of errors */

  class Invariant : Reference::Able {

  public:

    //! Default constructor
    Invariant ();

    //! Copy constructor
    Invariant (const Invariant&);

    //! Destructor
    ~Invariant ();

    //! Assignment operator
    Invariant& operator = (const Invariant&);

    //! Set the Stokes parameters
    void set_Stokes (const Stokes< Estimate<float> >&);

    //! Set the Stokes parameters
    void set_Stokes (const Stokes< Estimate<double> >&);

    //! Get the invariant interval
    Estimate<double> get_invariant () const;

    //! Get the estimated bias due to measurement error
    double get_bias () const;

  protected:

    //! The input Stokes parameters
    Stokes<ScalarMath> input;

    //! The invariant interval, I^2 - Q^2 - U^2 - V^2
    ScalarMath result;

    //! The bias due to measurement error
    double bias;

  private:

    //! Does the work for the constructors
    void init ();

  };

}

#endif

