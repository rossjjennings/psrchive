//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/HybridCalibrator.h,v $
   $Revision: 1.1 $
   $Date: 2003/12/05 20:21:04 $
   $Author: straten $ */

#ifndef __HybridCalibrator_H
#define __HybridCalibrator_H

#include "Pulsar/PolnCalibrator.h"

namespace Pulsar {

  class ArtificialCalibrator;
  class CalibratorStokes;

  //! Uses a SingleAxisModel to supplement another system response
  class HybridCalibrator : public PolnCalibrator {
    
  public:

    //! Construct from another PolnCalibrator instance
    HybridCalibrator (PolnCalibrator* precalibrator = 0);

    //! Destructor
    ~HybridCalibrator ();

    //! Return Calibrator::Hybrid
    Type get_type () const;

    //! Set the Stokes parameters of the reference signal
    void set_reference_input (CalibratorStokes* reference);

    //! Set the ArtificialCalibrator data from which to derive a SingleAxis
    void set_reference_observation (ArtificialCalibrator* observation);
    
    //! Set the PolnCalibrator to be supplemented
    void set_precalibrator (PolnCalibrator* calibrator);

  protected:

    //! Fill the transformation vector of the PolnCalibrator base class
    void calculate_transformation ();

    //! The Stokes parameters of the input reference signal
    Reference::To<CalibratorStokes> reference_input;

    //! The Stokes parameters of the observed reference signal
    Reference::To<ArtificialCalibrator> reference_observation;

    //! The PolnCalibrator to be supplemented
    Reference::To<PolnCalibrator> precalibrator;

  };

}

#endif
