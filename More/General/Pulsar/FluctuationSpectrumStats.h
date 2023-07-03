//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FluctuationSpectrumStats.h

#ifndef __Pulsar_FluctuationSpectrumStats_h
#define __Pulsar_FluctuationSpectrumStats_h

#include "Pulsar/FluctSpectStats.h"
#include "Pulsar/ProfileStats.h"
#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Computes statistics of the fluctuation spectrum (Fourier transform of pulse profile)
  class FluctuationSpectrumStats : public FluctSpectStats<Profile,ProfileStats>
  {   
  public:

    //! Returns the variance of the baseline
    std::complex< Estimate<double> > get_baseline_variance () const;

    //! Returns the total variance of the noise
    double get_noise_variance () const;

  protected:

    const Profile* reference (const Profile* profile) { return profile; }

  };

}

#endif



