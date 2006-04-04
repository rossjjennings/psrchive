//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/NoiseStatistics.h,v $
   $Revision: 1.1 $
   $Date: 2006/04/04 21:14:54 $
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
    void set_baseline_extent (float width);

    //! Return the noise to Fourier noise ratio
    float get_nfnr (const Profile* profile);

  protected:

    //! The fractional number of high frequencies used to calculate noise
    float baseline_extent;

  };

}

#endif
