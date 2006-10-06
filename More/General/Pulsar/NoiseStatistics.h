//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/NoiseStatistics.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_NoiseStatistics_h
#define __Pulsar_NoiseStatistics_h

#include "ReferenceAble.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio in the Fourier domain
  class NoiseStatistics : public Reference::Able {

  public:

    //! Default constructor
    NoiseStatistics ();

    //! Set the fractional number of high frequencies used to calculate noise
    void set_baseline_fourier (float width);

    //! Set the fractional number of phase bins used to calculate noise
    void set_baseline_time (float width);

    //! Return the noise to Fourier noise ratio
    float get_nfnr (const Profile* profile);

  protected:

    float baseline_fourier;
    float baseline_time;

  };

}

#endif
