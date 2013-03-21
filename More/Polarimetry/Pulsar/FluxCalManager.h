//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/FluxCalManager.h

#ifndef __Calibration_FluxCalManager_H
#define __Calibration_FluxCalManager_H

#include "Pulsar/SourceEstimate.h"
#include "Pulsar/BackendEstimate.h"
#include "Pulsar/SignalPath.h"

namespace Calibration
{
  //! Manages flux calibrator source state, signal path, and initial estimates
  class FluxCalObservation : public Reference::Able
  {
  public:

    void update ();

    //! Backend transformation for this observation
    Reference::To<BackendEstimate> backend;

    //! Model of source and its first guess/best estimate
    Reference::To<SourceEstimate> source;
  };

  //! Manages a pair of on-source and off-source flux calibrator observations
  class FluxCalPair : public Reference::Able
  {
  public:

    void update ();

    //! On-source observation
    Reference::To<FluxCalObservation> on;

    //! Off-source observation
    Reference::To<FluxCalObservation> off;
  };

  //! Manages multiple flux calibrator observations

  /*!
    Whereas pulsar and reference source (noise diode) are assumed to
    be observed around the same time, the flux calibrator observation
    may be observed at a different time; therefore, for each flux
    calibrator observation, a unique backend transformation instance
    is created that is disengaged from any temporal variations.
  */

  class FluxCalManager : public Reference::Able
  {
  public:

    //! Default constructor
    FluxCalManager (SignalPath* composite);

    //! Set the backend transformation to be cloned for each flux calibrator
    void set_backend (const MEAL::Complex2*);

    //! Add a new observation
    void add_observation (Signal::Source type);

    //! Integrate an estimate of the backend
    void integrate (Signal::Source type, const MEAL::Complex2*);

    //! Integrate an estimate of the source
    void integrate (const Jones< Estimate<double> >& correct,
		    const SourceObservation& data);

    void submit (CoherencyMeasurementSet&, const Stokes< Estimate<double> >&);

    //! Update all backend models with current best estimate
    void update ();

    //! Return true if at least one complete flux calibrator observation
    bool is_constrained () const;

    bool multiple_source_states;
    bool subtract_off_from_on;

  protected:

    //! Add a new backend to the model
    void add_backend (FluxCalObservation*);

    //! Add a new source to the model
    void add_source (FluxCalObservation*);

    //! The backend transformation to be cloned for each flux calibrator
    Reference::To< MEAL::Complex2 > backend;

    //! The frontend component of the instrument
    Reference::To< MEAL::ProductRule<MEAL::Complex2> > frontend;
    
    //! The SignalPath manager into which signal paths are added
    Reference::To< SignalPath > composite;

    //! The set of flux calibrator observations
    std::vector< Reference::To<FluxCalPair> > observations;

  };

}

#endif
