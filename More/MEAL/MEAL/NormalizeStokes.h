//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/NormalizeStokes.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_NormalizeStokes_h
#define __MEAL_NormalizeStokes_h

#include "Stokes.h"
#include "Estimate.h"
#include "ScalarMath.h"

namespace MEAL {

  class Scalar;

  //! Normalize Stokes parameters by the invariant interval
  /*! This class properly handles the propagation of errors during
    normalization. */

  class NormalizeStokes : Reference::Able {

  public:

    //! Default constructor
    NormalizeStokes ();

    //! Copy constructor
    NormalizeStokes (const NormalizeStokes&);

    //! Destructor
    ~NormalizeStokes ();

    //! Assignment operator
    NormalizeStokes& operator = (const NormalizeStokes&);

    //! Normalize the Stokes parameters by their invariant interval
    void normalize (Stokes< Estimate<float> >& stokes);

    //! Normalize the Stokes parameters by their invariant interval
    void normalize (Stokes< Estimate<double> >& stokes);

  protected:

    //! The input Stokes parameters
    Stokes<ScalarMath> input;

    //! The output Stokes parameters
    Stokes<ScalarMath> output;

  private:

    //! Does the work for the constructors
    void init ();

  };

}

#endif

