//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarInverseTangent2.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_ScalarInverseTangent2_H
#define __Model_ScalarInverseTangent2_H

#include "MEPL/BinaryScalar.h"

namespace Model {

  //! The tangent of a Scalar function
  class ScalarInverseTangent2 : public BinaryScalar
  {

  public:

    // ///////////////////////////////////////////////////////////////////
    //
    // BinaryScalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! The function
    double function (double arg1, double arg2) const;

    //! The partial derivative with respect to arg1
    double partial_arg1 (double arg1, double arg2) const;

    //! The partial derivative with respect to arg2
    double partial_arg2 (double arg1, double arg2) const;

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
