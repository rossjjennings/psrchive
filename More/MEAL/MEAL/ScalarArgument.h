//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarArgument.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:16 $
   $Author: straten $ */

#ifndef __ScalarArgument_H
#define __ScalarArgument_H

#include "Calibration/Scalar.h"
#include "Calibration/Univariate.h"
#include "Calibration/NoParameters.h"

namespace Calibration {

  //! Represents a constant scalar value
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
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    string get_name () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Scalar implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the argument
    double evaluate (vector<double>* gradient=0) const;
    
  };

}

#endif
