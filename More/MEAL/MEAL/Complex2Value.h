//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Complex2Value.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:14 $
   $Author: straten $ */

#ifndef __Complex2Value_H
#define __Complex2Value_H

#include "Calibration/OptimizedComplex2.h"

namespace Calibration {

  //! Represents a complex 2x2 matrix value, \f$ J \f$
  class Complex2Value : public OptimizedComplex2 {

  public:

    //! Default constructor
    Complex2Value (const Jones<double>& value = 1.0);

    //! Set the value
    void set_value (const Jones<double>& value);

    //! Get the value
    Jones<double> get_value () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

    //! Return the name of the parameter
    string get_param_name (unsigned i) const { return ""; }

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedModel implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the value (and gradient, if requested) of the function
    void calculate (Jones<double>& x, vector< Jones<double> >* grad=0);

    //! The value
    Jones<double> value;

  };

}

#endif
