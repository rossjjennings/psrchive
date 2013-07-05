//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Spinor.h,v $
   $Revision: 1.8 $
   $Date: 2009/06/06 11:07:20 $
   $Author: straten $ */

#ifndef __MEAL_Spinor_H
#define __MEAL_Spinor_H

#include "MEAL/Evaluable.h"
#include "Vector.h"
#include <complex>

namespace MEAL
{
  //! Pure virtual base class of scalar functions
  class Spinor : public Evaluable< Vector< 2, std::complex<double> > >
  {
  public:

    //! The name of the class
    static const char* Name;

    //! Clone method
    virtual Spinor* clone () const;

  };
}

#endif
