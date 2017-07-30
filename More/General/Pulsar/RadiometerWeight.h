//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/RadiometerWeight.h

#ifndef _Pulsar_RadiometerWeight_H
#define _Pulsar_RadiometerWeight_H

#include "Pulsar/IntegrationWeight.h"

namespace Pulsar {
  
  //! Sets the weight of each Integration according to its duration
  class RadiometerWeight : public IntegrationWeight
  {
    double get_weight (const Integration*, unsigned ichan);
  };
  
}

#endif

