//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Rotation.h

#ifndef __MEAL_Rotation_H
#define __MEAL_Rotation_H

#include "MEAL/Complex2.h"
#include "Vector.h"

namespace MEAL {

  //! Represents a rotation (passive unitary transformation) about a free axis
  /*! This class represents passive transformation that rotates the basis
    through an angle, \f$\phi\f$, about a unit vector, \f$\hat n\f$,
    as defined by the right-hand rule.

    When transformed by the rotation matrix returned by this object,
    a vector is rotated about the axis by \f$\- phi\f$.

    The three free parameters are the components of the rotation vector,
    \f${\bf r}=\phi\hat n\f$,
    which are related to the quaternion representation by

    \f$ Q = [\cos|{\bf r}|, i\, {\rm sinc}|{\bf r}| {\bf r}]\f$
  */
  class Rotation : public Complex2
  {

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
