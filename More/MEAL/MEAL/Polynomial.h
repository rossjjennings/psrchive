//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Polynomial.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Polynomial_H
#define __Polynomial_H

#include "MEPL/UnivariateOptimizedScalar.h"

namespace Model {

  //! Polynomial function with an arbitrary number of coefficients
  class Polynomial : public UnivariateOptimizedScalar {

  public:

    //! Default constructor
    Polynomial (unsigned ncoef = 0);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

    //! Return the name of the specified parameter
    string get_param_name (unsigned index) const;

    //! Parses the values of model parameters and fit flags from a string
    void parse (const string& text);

  protected:

    //! Print the values of model parameters and fit flags to a string
    void print_parameters (string& text, const string& sep) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Optimized implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the value (and gradient, if requested) of the function
    void calculate (double& x, vector<double>* grad=0);

  private:

    //! Abscissa offset
    double x_0;

  };

}

#endif
