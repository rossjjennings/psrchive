//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/ScalarConstant.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:16 $
   $Author: straten $ */

#ifndef __ScalarConstant_H
#define __ScalarConstant_H

#include "Calibration/ConstantModel.h"
#include "Calibration/Scalar.h"

namespace Calibration {

  //! Represents a constant scalar value
  class ScalarConstant : public ConstantModel, public Scalar {

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

    //! Return the value
    double evaluate (vector<double>* gradient=0) const;
    
  protected:

    //! The value returned by evaluate
    double value;

  };

}

#endif
