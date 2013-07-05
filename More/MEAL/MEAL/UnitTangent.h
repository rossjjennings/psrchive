//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/UnitTangent.h

#ifndef __MEAL_UnitTangent_H
#define __MEAL_UnitTangent_H

#include "MEAL/Evaluable.h"
#include "Vector.h"

namespace MEAL {

  //! Represents a unit vector in three-dimensional space

  /*! This model of a unit vector solves the gimbal lock issue by
    redefining the basis as it moves.  At any point, the coordinates
    of the vector are defined by (0,0).  A step in the any of the two
    possible directions is defined as a step in the plane that is
    tangent to the unit sphere.  

    After such a step, when the evaluate method is called, the vector
    is renormalized, the unit vectors in the tangent plane are
    redefined, and the coordinates are reset to (0,0).

    When choosing the new unit vectors in the tangent plane, the
    direction to the most distant axis (in Cartesian coordinates) is
    used to define the first direction.

    Note that, in this representation, it is impossible to rotate the
    unit vector by more than 90 degrees in one step.  This should
    also provide some stability during modelling.
  */

  class UnitTangent : public Evaluable< Vector<3,double> >
  {

  public:

    UnitTangent ();

    //! Return the name of the class
    std::string get_name () const;

    //! Set the direction of the unit vector
    void set_vector (Vector<3,double> direction);

    //! Set the direction of the unit vector with error
    void set_vector (const Vector<3,Estimate<double> >& direction);

    //! Get the basis vector
    Vector<3,double> get_basis (unsigned i) { return basis[i]; }

  protected:

    //! Calculate the Vector and its gradient
    void calculate (Vector<3,double>& result, std::vector<Vector<3,double> >*);

    //! The current basis
    Vector<3,double> basis [3];

  };

}

#endif
