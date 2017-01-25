//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Real4.h

#ifndef __MEAL_Real4_H
#define __MEAL_Real4_H

#include "MEAL/Evaluable.h"
#include "Matrix.h"

namespace MEAL {

  //! Pure virtual base class of all real-valued 4x4 matrix functions
  class Real4 : public Evaluable< Matrix<4,4,double> > 
  {
  public:

    //! The name of the class
    static const char* Name;

    //! Clone method
    virtual Real4* clone () const;
  };

}

#endif
