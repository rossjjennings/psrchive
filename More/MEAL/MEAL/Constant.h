//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Constant.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 19:26:03 $
   $Author: straten $ */

#ifndef __Constant_H
#define __Constant_H

#include "MEAL/NoParameters.h"

namespace MEAL {

  //! Represents a constant model with no parameters
  /*! The Constant primarily serves as an optimization convention.
    The Composite will not map a Constant, thereby reducing the number
    of Function instances managed by the Composite and improving performance */

  class Constant : public NoParameters {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////
 
    //! Return true if the result has changed
    bool get_changed () const { return false; }

  };

}

#endif
