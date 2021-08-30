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

  class ScalarParameter;

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

    //! kappa: inverse of slope at magnetic meridian
    Reference::To<ScalarParameter> inverse_slope;

    //! zeta: colatitude of line of sight with respect to spin axis
    Reference::To<ScalarParameter> line_of_sight;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  private:

    void init ();
  };

}

#endif
