//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarValue.h,v $
   $Revision: 1.5 $
   $Date: 2005/04/24 01:11:19 $
   $Author: straten $ */

#ifndef __ScalarValue_H
#define __ScalarValue_H

#include "MEAL/Scalar.h"

namespace MEAL {

  //! Represents a scalar value with no parameters
  class ScalarValue : public Scalar {

  public:

    //! Default constructor
    ScalarValue (double value = 1.0);

    //! Set the value
    void set_value (double value);

    //! Get the value
    double get_value () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Return the value (and gradient, if requested) of the function
    void calculate (double& x, std::vector<double>* grad=0);

    //! The value
    double value;

  };

}

#endif
