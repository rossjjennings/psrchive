//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarConstant.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __ScalarConstant_H
#define __ScalarConstant_H

#include "MEAL/Constant.h"
#include "MEAL/Scalar.h"

namespace MEAL {

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
    std::string get_name () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Scalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the value
    double evaluate (std::vector<double>* gradient=0) const;
    
  protected:

    //! The value returned by evaluate
    double value;

  };

}

#endif
