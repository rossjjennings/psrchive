//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarValue.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:17 $
   $Author: straten $ */

#ifndef __ScalarValue_H
#define __ScalarValue_H

#include "Calibration/OptimizedScalar.h"

namespace Calibration {

  //! Represents a scalar value, \f$ x \f$
  class ScalarValue : public OptimizedScalar {

  public:

    //! Default constructor
    ScalarValue (Estimate<double> value = 1.0);

    void set_value (const Estimate<double>& value);
    Estimate<double> get_value () const;

    //! Get the name of the parameter
    string get_value_name () const;

    //! Set the name of the parameter
    void set_value_name (const string& name);

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

    //! Return the name of the specified parameter
    string get_param_name (unsigned index) const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedModel implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the value (and gradient, if requested) of the function
    void calculate (double& x, vector<double>* grad=0);

    //! The name of the value
    string value_name;

  };

}

#endif

