//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Types/Pulsar/CalibratorTypeInterface.h

#ifndef __Pulsar_CalibratorTypeInterface_h
#define __Pulsar_CalibratorTypeInterface_h

#include "Pulsar/CalibratorType.h"
#include "TextInterface.h"

namespace Pulsar
{
  //! Text interface to Calibrator::Type class
  class Calibrator::Type::Interface
    : public TextInterface::To<const Calibrator::Type>
  {
  public:
    Interface (const Calibrator::Type* = 0);
  };

}

#endif
