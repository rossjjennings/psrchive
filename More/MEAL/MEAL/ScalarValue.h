//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarValue.h,v $
   $Revision: 1.4 $
   $Date: 2005/04/06 20:23:36 $
   $Author: straten $ */

#ifndef __ScalarValue_H
#define __ScalarValue_H

#include "MEAL/Scalar.h"

namespace MEAL {

  //! Represents a scalar value, \f$ x \f$
  class ScalarValue : public Scalar {

  public:

    //! Default constructor
    ScalarValue (Estimate<double> value = 1.0);

    void set_value (const Estimate<double>& value);
    Estimate<double> get_value () const;

    //! Get the name of the parameter
    std::string get_value_name () const;

    //! Set the name of the parameter
    void set_value_name (const std::string& name);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Optimized implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the value (and gradient, if requested) of the function
    void calculate (double& x, std::vector<double>* grad=0);

    //! The name of the value
    std::string value_name;

  };

}

#endif

