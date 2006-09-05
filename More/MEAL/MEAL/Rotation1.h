//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Rotation1.h,v $
   $Revision: 1.1 $
   $Date: 2006/09/05 18:15:03 $
   $Author: straten $ */

#ifndef __MEAL_Rotation1_H
#define __MEAL_Rotation1_H

#include "MEAL/Complex2.h"
#include "Vector.h"

namespace MEAL {

  class OneParameter;

  //! Represents a rotation (unitary, birefringent) transformation
  /*! This class represents a rotation through angle, \f$\phi\f$,
    about a fixed axis, \f$\hat n\f$. */
  class Rotation1 : public Complex2 {

  public:

    //! Construct for the specified axis
    Rotation1 (const Vector<3,double>& axis);

    //! Set the axis about which the rotation occurs
    void set_axis (const Vector<3,double>& axis);

    //! Get the axis about which the rotation occurs
    Vector<3,double> get_axis () const;

    //! Set the rotation angle in radians
    void set_phi (const Estimate<double>& radians);

    //! Get the rotation angle in radians
    Estimate<double> get_phi () const;

    //! Set the parameter management policy
    void set_parameter_policy (OneParameter* policy);

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

    //! The axis along which the rotation occurs
    Vector<3,double> axis;

  };

}

#endif
