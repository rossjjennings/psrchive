//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarArgument.h,v $
   $Revision: 1.4 $
   $Date: 2004/11/23 12:14:35 $
   $Author: straten $ */

#ifndef __ScalarArgument_H
#define __ScalarArgument_H

#include "MEAL/Scalar.h"
#include "MEAL/Univariate.h"
#include "MEAL/NoParameters.h"

namespace MEAL {

  //! Represents a scalar argument
  class ScalarArgument : public Scalar,public Univariate,public NoParameters {

  public:

    //! Default constructor
    ScalarArgument ();

    //! Copy constructor
    ScalarArgument (const ScalarArgument& scalar);

    //! Assignment operator
    const ScalarArgument& operator = (const ScalarArgument& scalar);

    //! Destructor
    ~ScalarArgument ();

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

    //! Return the argument
    double evaluate (std::vector<double>* gradient=0) const;
    
  };

}

#endif
