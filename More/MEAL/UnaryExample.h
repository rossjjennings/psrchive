//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/UnaryExample.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_UnaryExample_H
#define __Model_UnaryExample_H

#include "MEPL/UnaryScalar.h"

namespace Model {

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
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

  };

}


#endif
