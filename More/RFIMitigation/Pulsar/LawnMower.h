//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/RFIMitigation/Pulsar/LawnMower.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/31 05:45:08 $
   $Author: straten $ */

#ifndef __Pulsar_LawnMower_h
#define __Pulsar_LawnMower_h

#include "Pulsar/Transformation.h"
#include "Pulsar/Integration.h"
#include "Functor.h"

namespace Pulsar {

  class BaselineEstimator;
  class OnPulseThreshold;
  class PhaseWeight;

  //! Removes broad-band impulsive spikes from pulse profiles
  /*! By adding preconditions, this class can be made interactive */
  class LawnMower : public Transformation<Integration> {

  public:

    //! Default constructor
    LawnMower ();

    //! Destructor
    ~LawnMower ();

    //! Set the Profile from which baseline PhaseWeight will be computed
    void transform (Integration*);

    //! Set the BaselineEstimator used to find the baseline
    void set_baseline_estimator (BaselineEstimator*);
    BaselineEstimator* get_baseline_estimator () const;

    //! Set the threshold above which samples are mown
    virtual void set_threshold (float sigma);

    //! If set, search for spikes in fscrunched (DM=0) total
    virtual void set_broadband (bool);

    //! One or more preconditions can be added
    virtual void add_precondition( Functor< bool(Profile*,PhaseWeight*) > );

  protected:
    
    //! Masks all points above a threshold
    Reference::To<OnPulseThreshold> mower;

    //! Mask
    Reference::To<PhaseWeight> mask;

    //! Create mask and return true if all preconditions are satisfied
    bool build_mask (Profile* profile);

    //! Search for spikes in fscrunch (DM=0) total
    bool broadband;

    //! Preconditions
    std::vector< Functor< bool(Profile*,PhaseWeight*) > > precondition;

  };

}

#endif
