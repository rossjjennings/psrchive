//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/IonosphereCalibrator.h

#ifndef __Pulsar_IonosphereCalibrator_H
#define __Pulsar_IonosphereCalibrator_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  //! Corrects ionospheric Faraday rotation
  /*! Once upon a time, we tried.  */
  class IonosphereCalibrator : public Calibrator
  {

  public:

    //! Default constructor
    IonosphereCalibrator ();

    //! Destructor
    ~IonosphereCalibrator ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Calibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calibrate the Pulsar::Archive
    void calibrate (Archive* archive);

    //! Get the number of frequency channels in the calibrator
    unsigned get_nchan () const { return 0; }

    //! Get the weight associated with the specified channel
    float get_weight (unsigned ichan) const { return 1.0; }

    //! Returns a new CalibratorExtension
    CalibratorExtension* new_Extension () const { return 0; }
  };
}

#endif
