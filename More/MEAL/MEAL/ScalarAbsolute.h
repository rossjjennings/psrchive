//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarAbsolute.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:16 $
   $Author: straten $ */

#ifndef __Calibration_ScalarAbsolute_H
#define __Calibration_ScalarAbsolute_H

#include "Calibration/UnaryScalar.h"

namespace Calibration {

  //! The absolute value of a Scalar function
  class ScalarAbsolute : public UnaryScalar
  {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // UnaryScalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! The function
    double function (double x) const;

    //! And its first derivative
    double dfdx (double x) const;

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
