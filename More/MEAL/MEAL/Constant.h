//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Constant.h,v $
   $Revision: 1.3 $
   $Date: 2005/04/06 20:20:54 $
   $Author: straten $ */

#ifndef __MEAL_Constant_H
#define __MEAL_Constant_H

#include "MEAL/NoParameters.h"

namespace MEAL {

  //! Represents a constant model with no parameters
  /*! The Constant primarily serves as an optimization convention.
    The Composite will not map a Constant, thereby reducing the number
    of Function instances managed by the Composite and improving performance */

  class Constant : public NoParameters {

  public:


  };

}

#endif
