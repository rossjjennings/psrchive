//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Rotation.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_Rotation_H
#define __MEAL_Rotation_H

#include "MEAL/OptimizedComplex2.h"
#include "Vector.h"

namespace MEAL {

  //! Represents a rotation (unitary, birefringent) transformation
  /*! This class represents the rotation, \f$\phi\f$, about an arbitrary
    axix, \f$\hat n\f$. */
  class Rotation : public OptimizedComplex2 {

  public:

    //! Default constructor
    Rotation ();

    //! Construct for the specified axis
    Rotation (const Vector<double, 3>& axis);

    //! Set the axis about which the rotation occurs
    void set_axis (const Vector<double, 3>& axis);

    //! Set the rotation angle in radians
    void set_phi (double radians);

    //! Get the rotation angle in radians
    double get_phi () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Return the name of the specified parameter
    std::string get_param_name (unsigned index) const;

  protected:

    //! The axis along which the rotation occurs
    Vector<double, 3> axis;

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedComplex2 implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >* gradient);

  };

}

#endif
