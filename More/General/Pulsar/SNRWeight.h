//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/SNRWeight.h

#ifndef _Pulsar_SNRWeight_H
#define _Pulsar_SNRWeight_H

#include "Pulsar/IntegrationWeight.h"

namespace Pulsar {
  
  //! Sets the weight of each Profile according to its S/N squared
  class SNRWeight : public IntegrationWeight {

  public:

    //! Default constructor
    SNRWeight ();

    //! Set weight to zero if S/N falls below threshold
    void set_threshold (float f) { threshold = f; }
    float get_threshold () const { return threshold; }

    //! Get the weight of the specified channel
    double get_weight (const Integration* integration, unsigned ichan);

  protected:

    float threshold;

  };
  
}

#endif

