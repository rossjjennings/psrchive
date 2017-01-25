//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/DurationWeight.h

#ifndef _Pulsar_DurationWeight_H
#define _Pulsar_DurationWeight_H

#include "Pulsar/IntegrationWeight.h"

namespace Pulsar {
  
  //! Sets the weight of each Integration according to its duration
  class DurationWeight : public IntegrationWeight
  {
    double get_weight (const Integration*, unsigned ichan);
  };
  
}

#endif

