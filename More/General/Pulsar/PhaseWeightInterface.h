//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/PhaseWeightInterface.h

#ifndef __Pulsar_PhaseWeightTI_h
#define __Pulsar_PhaseWeightTI_h

#include "Pulsar/PhaseWeight.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Archive statistics text interface
  class PhaseWeight::Interface : public TextInterface::To<PhaseWeight>
  {
  public:

    //! Default constructor
    Interface ( PhaseWeight* = 0 );

  };

}


#endif
