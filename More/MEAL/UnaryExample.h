//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/UnaryExample.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:17 $
   $Author: straten $ */

#ifndef __Calibration_UnaryExample_H
#define __Calibration_UnaryExample_H

#include "Calibration/UnaryScalar.h"

namespace Calibration {

  //! Example of a UnaryScalar child
  class UnaryExample : public UnaryScalar
  {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // UnaryScalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! The function
    double function (double x);

    //! And its first derivative
    double dfdx (double x);


    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

  };

}


#endif
