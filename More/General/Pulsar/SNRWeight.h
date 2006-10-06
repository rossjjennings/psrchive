//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SNRWeight.h,v $
   $Revision: 1.2 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

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

  protected:

    //! Set integration weights
    void weight (Integration* integration);

    float threshold;

  };
  
}

#endif

