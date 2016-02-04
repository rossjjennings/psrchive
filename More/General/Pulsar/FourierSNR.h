//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/FourierSNR.h

#ifndef __Pulsar_FourierSNR_h
#define __Pulsar_FourierSNR_h

#include "Pulsar/SNRatioEstimator.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio in the Fourier domain
  class FourierSNR : public SNRatioEstimator {

  public:

    //! Default constructor
    FourierSNR ();

    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! The class that is returned by get_interface
    class Interface;

    //! Return a copy constructed instance of self
    FourierSNR* clone () const;

    //! Set the fractional number of high frequencies used to calculate noise
    void set_baseline_extent (float width);
    float get_baseline_extent () const { return baseline_extent; }

  protected:

    //! The fractional number of high frequencies used to calculate noise
    float baseline_extent;

  };

}

#endif
