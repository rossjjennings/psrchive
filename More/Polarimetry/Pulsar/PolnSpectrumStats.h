//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/PolnSpectrumStats.h

#ifndef __Pulsar_PolnSpectrumStats_h
#define __Pulsar_PolnSpectrumStats_h

#include "Pulsar/FluctSpectStats.h"
#include "Pulsar/PolnProfileStats.h"
#include "Pulsar/PolnProfile.h"

namespace Pulsar {

  //! Computes statistics of full-polarization fluctuation spectra
  class PolnSpectrumStats : public FluctSpectStats<PolnProfile,PolnProfileStats>
  {   
  public:

    //! Get the Stokes parameters for the specified harmonic
    Stokes< std::complex< Estimate<double> > > get_stokes (unsigned) const;

    //! Returns the total determinant of the on-pulse phase bins
    Estimate<double> get_total_determinant () const;

    //! Returns the variance of the baseline for the specified polarization
    std::complex< Estimate<double> > get_baseline_variance (unsigned) const;

  protected:

    //! Ensure that the PolnProfile is in the Stokes state
    void preprocess (PolnProfile*);

    //! Return the total intensity profile
    const Profile* reference (const PolnProfile* profile) { return profile->get_Profile(0); }

  };

}

#endif
