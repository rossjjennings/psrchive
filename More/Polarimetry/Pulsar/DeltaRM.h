//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 - 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/DeltaRM.h

#ifndef __Pulsar_DeltaRM_h
#define __Pulsar_DeltaRM_h

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/FrequencyIntegrate.h"

#include "Estimate.h"

namespace Pulsar {

  class Archive;

  //! Refines an RM estimate using two halves of the band
  class DeltaRM  {

  public:

    //! Default constructor
    DeltaRM ();

    //! Destructor
    ~DeltaRM ();

    //! Set the rotation measure (initial guess)
    void set_rotation_measure (const Estimate<double>& rm)
    { rotation_measure = rm; }

    //! Get the rotation measure 
    Estimate<double> get_rotation_measure () const
    { return rotation_measure; }

    //! Get the number of phase bins used in last call to refine
    unsigned get_used_bins () { return used_bins; }

    //! Set the archive from which to derive the refined rotation measure
    void set_data (Archive*);

    //! Refine the rotation measure estimate
    void refine ();

    //! Set the cutoff threshold in units of the baseline noise
    void set_threshold (float t) { threshold = t; }
    float get_threshold () const { return threshold; }

     //! Set the on-pulse mask
    void set_onpulse (PhaseWeight* w) { onpulse_weights = w; }
    
   //! Set the phase bins to be included in the computation
    void set_include (const std::vector<unsigned>& bins);

    //! Set the phase bins to be excluded from the computation
    void set_exclude (const std::vector<unsigned>& bins);

    //! Set the policy used to integrate frequency channels
    void set_policy (FrequencyIntegrate::Divided*);

  protected:

    float threshold;
    unsigned used_bins;

    //! Include only the specified phase bins
    std::vector<unsigned> include_bins;

    //! Exclude the specified phase bins
    std::vector<unsigned> exclude_bins;

    //! The rotation measure
    Estimate<double> rotation_measure;

    //! The archive from which the rotation measure will be refined
    Reference::To<Archive> data;

    //! Weights applied to each phase bin (defines on-pulse region)
    Reference::To<PhaseWeight> onpulse_weights;

    //! Policy used to integrate frequency channels
    Reference::To<FrequencyIntegrate::Divided> policy;
  };

}

#endif
