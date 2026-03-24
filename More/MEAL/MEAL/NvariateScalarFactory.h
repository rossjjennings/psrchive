//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __MEAL_NvariateScalarFactory_h
#define __MEAL_NvariateScalarFactory_h

#include "MEAL/Nvariate.h"
#include "MEAL/Scalar.h"

namespace MEAL {

  //! Constructs and configures new Nvariate<Scalar> given descriptive string
  class NvariateScalarFactory 
  {
  public:

    //! Construct a new transformation
    Nvariate<Scalar>* operator () (const std::string&);

  };
}

#endif
