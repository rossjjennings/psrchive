//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Rotation.h,v $
   $Revision: 1.9 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __MEAL_Rotation_H
#define __MEAL_Rotation_H

#include "MEAL/Complex2.h"
#include "Vector.h"

namespace MEAL {

  class OneParameter;

  //! Represents a rotation (unitary, birefringent) transformation
  /*! This class represents a rotation through angle, \f$\phi\f$,
    about a free axis, \f$\hat n\f$.  The three free parameters are
    the imaginary components of the rotation quaternion. */
  class Rotation : public Complex2 {

  public:

    //! Default constructor
    Rotation ();

    //! Get the unit-vector along which the rotation occurs
    Vector<3, double> get_axis () const;

    //! Get the rotation angle, phi
    double get_phi () const;

    //! Get the three free parameters as a vector
    Vector<3, double> get_vector () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

  };

}

#endif
