//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SquareWave.h,v $
   $Revision: 1.3 $
   $Date: 2006/04/06 20:22:11 $
   $Author: straten $ */

#ifndef __Pulsar_SquareWave_h
#define __Pulsar_SquareWave_h

#include "Pulsar/Algorithm.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio using (on-off)/rms
  class SquareWave : public Algorithm {

  public:

    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

  };

}

#endif
