//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 - 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/DeltaPA.h

#ifndef __Pulsar_DeltaPA_h
#define __Pulsar_DeltaPA_h

#include "Estimate.h"
#include "Pulsar/PhaseWeight.h"

#include <vector>

namespace Pulsar {

  class PolnProfile;

  //! Computes mean position angle difference from weighted cross-correlation
  class DeltaPA  {

  public:

    //! Default constructor
    DeltaPA () { threshold = 3.0; }

    //! Destructor
    ~DeltaPA () { }

    //! Get the position angle difference mean
    Estimate<double> get (const PolnProfile* p0, const PolnProfile* p1) const;

    //! Get the number of phase bins used in the last call to get
    unsigned get_used_bins () const { return used_bins; }

    //! Set the cutoff threshold in units of the baseline noise
    void set_threshold (float t) { threshold = t; }
    float get_threshold () const { return threshold; }

    //! Set the on-pulse mask
    void set_onpulse (PhaseWeight* w) { onpulse_weights = w; }
    
    //! Set the phase bins to be included in the mean
    void set_include (const std::vector<unsigned>& bins);

    //! Set the phase bins to be excluded from the mean
    void set_exclude (const std::vector<unsigned>& bins);

  protected:

    float threshold;
    mutable unsigned used_bins;

    //! Include only the specified phase bins
    std::vector<unsigned> include_bins;

    //! Exclude the specified phase bins
    std::vector<unsigned> exclude_bins;

    //! Weights applied to each phase bin (defines on-pulse region)
    Reference::To<PhaseWeight> onpulse_weights;
    
  };

}

#endif
