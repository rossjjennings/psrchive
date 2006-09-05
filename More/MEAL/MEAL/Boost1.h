//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Boost1.h,v $
   $Revision: 1.1 $
   $Date: 2006/09/05 18:14:37 $
   $Author: straten $ */

#ifndef __MEAL_Boost1_H
#define __MEAL_Boost1_H

#include "MEAL/Complex2.h"
#include "Vector.h"

namespace MEAL {

  //! Represents a boost (Hermitian, dichroic) transformation
  /*! This class represents the boost, \f$\beta\f$, along an arbitrary
    axix, \f$\hat m\f$. */
  class Boost1 : public Complex2 {

  public:

    //! Construct with a fixed axis
    Boost1 (const Vector<3,double>& axis);

    //! Set the axis along which the boost occurs
    void set_axis (const Vector<3,double>& axis);

    //! Get the unit-vector along which the boost occurs
    Vector<3,double> get_axis () const;

    //! Set the boost parameter, beta
    void set_beta (const Estimate<double>& beta);

    //! Get the boost parameter, beta
    Estimate<double> get_beta () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! The axis along which the boost occurs
    Vector<3,double> axis;

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

  };

}

#endif
