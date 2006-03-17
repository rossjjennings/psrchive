//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarValue.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/17 13:35:26 $
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
