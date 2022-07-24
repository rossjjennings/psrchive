//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/CalibrationInterpolator.h

#ifndef __CalibrationInterpolator_H
#define __CalibrationInterpolator_H

#include "Pulsar/Calibrator.h"
#include <map>

namespace Pulsar {

  class CalibrationInterpolatorExtension;
  class FluxCalibratorExtension;
  class PolnCalibratorExtension;
  class CalibratorStokes;
  class Integration;
  class SplineSmooth2D;
  
  //! A calibration solution that spans a finite bandwidth and time
  class CalibrationInterpolator : public Calibrator::Variation
  {
    //! Construct from an Archive with a CalibrationInterpolatorExtension
    void construct (Archive*);
 
  public:

    //! Construct from a calibrator with a CalibrationInterpolatorExtension
    CalibrationInterpolator (Calibrator*);

    //! Construct from an Archive with a CalibrationInterpolatorExtension
    CalibrationInterpolator (Archive*);

    //! Destructor
    ~CalibrationInterpolator ();

    //! Get the extension from which this object was constructed
    const CalibrationInterpolatorExtension* get_extension();

    //! Get the type of the calibrator
    const Calibrator::Type* get_type () const;

    //! Update the model parameters to match the Integration
    bool update (const Integration* subint)
    { return update<Integration>(subint); }

    //! Update the model parameters to match the container
    /*! Returns true if transformation should be recomputed */
    template<class Container>
    bool update (const Container*);
    
  protected:

    //! Calibration Interpolator Extension
    Reference::To<const CalibrationInterpolatorExtension> interpolator;
    
    //! The Stokes parameters of the input reference signal
    Reference::To<CalibratorStokes> calpoln;

    //! The model of the feed
    Reference::To<PolnCalibratorExtension> feedpar;

    //! The flux calibrator
    Reference::To<FluxCalibratorExtension> fluxcal;

    std::map< unsigned, Reference::To<SplineSmooth2D> > feedpar_splines;
    std::map< unsigned, Reference::To<SplineSmooth2D> > calpoln_splines;
    std::map< unsigned, Reference::To<SplineSmooth2D> > fluxcal_splines;

    MJD last_computed;

    //! Performs the work for the template update method
    bool update (const MJD& epoch, const std::vector<double>& freqs);
  };

  template<class Container>
  bool CalibrationInterpolator::update (const Container* container)
  {
    unsigned nchan = container->get_nchan();
    std::vector<double> frequency (nchan);
  
    for (unsigned ichan=0; ichan<nchan; ++ichan)
      frequency[ichan] = container->get_centre_frequency (ichan);
  
    return update (container->get_epoch (), frequency);
  }

}

#endif
