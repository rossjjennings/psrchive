//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/ScalarArgument.h

#ifndef __ScalarArgument_H
#define __ScalarArgument_H

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"

namespace MEAL {

  //! Represents a scalar argument, \f$ a \f$
  class ScalarArgument : public Univariate<Scalar> {

  public:

    //! Default constructor
    ScalarArgument ();

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Return the argument
    void calculate (double& result, std::vector<double>* gradient=0);
    
  };

}

#endif
