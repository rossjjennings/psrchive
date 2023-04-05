//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/OrthoRVM.h

#ifndef __MEAL_OrthoRVM_H
#define __MEAL_OrthoRVM_H

#include "MEAL/RVM.h"

namespace MEAL {

  //! Orthometric Rotating Vector Model (RVM)
  class OrthoRVM : public RVM
  {

  public:

    //! Default constructor
    OrthoRVM ();

    //! Copy constructor
    OrthoRVM (const OrthoRVM& copy);

    //! Assignment operator
    OrthoRVM& operator = (const OrthoRVM& copy);

    //! Destructor
    ~OrthoRVM ();

    //! colatitude of line of sight with respect to spin axis
    void set_line_of_sight (double radians);
    Estimate<double> get_line_of_sight () const;
 
    //! kappa = 1/(dPsi/dphi) (inverse of slope at magnetic meridian)
    Reference::To<ScalarParameter> kappa;

    //! lambda = cot(zeta) (cotangent of colatitude of the line of sight)
    Reference::To<ScalarParameter> lambda; 

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  private:

    ScalarMath cos_zeta;
    void init ();
  };

}

#endif
