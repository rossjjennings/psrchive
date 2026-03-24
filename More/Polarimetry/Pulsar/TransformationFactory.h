//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Calibration_TransformationFactory_h
#define __Calibration_TransformationFactory_h

#include "MEAL/Complex2.h"

namespace Calibration {

  //! Constructs and configures new Complex2 given descriptive string
  class TransformationFactory 
  {
  public:

    //! Construct a new transformation
    virtual MEAL::Complex2* operator() (const std::string&);
  };
}

#endif
