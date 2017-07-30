//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/BackendInterface.h

#ifndef __Pulsar_BackendInterface_h
#define __Pulsar_BackendInterface_h

#include "Pulsar/Backend.h"
#include "TextInterface.h"

namespace Pulsar
{
  //! Text interface to Backend extension
  class Backend::Interface : public TextInterface::To<Pulsar::Backend>
  {
  public:
    Interface (Backend* = 0);
  };

}

#endif
