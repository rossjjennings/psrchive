//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarConstant.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __ScalarConstant_H
#define __ScalarConstant_H

#include "MEPL/Constant.h"
#include "MEPL/Scalar.h"

namespace Model {

  //! Represents a constant scalar value
  class ScalarConstant : public Constant, public Scalar {

  public:

    //! Default constructor
    ScalarConstant (double value);

    //! Copy constructor
    ScalarConstant (const ScalarConstant& scalar);

    //! Assignment operator
    const ScalarConstant& operator = (const ScalarConstant& scalar);

    //! Destructor
    ~ScalarConstant ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Scalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the value
    double evaluate (vector<double>* gradient=0) const;
    
  protected:

    //! The value returned by evaluate
    double value;

  };

}

#endif
