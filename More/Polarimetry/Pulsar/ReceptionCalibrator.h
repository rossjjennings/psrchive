//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.4 $
   $Date: 2003/04/19 20:21:27 $
   $Author: straten $ */

#ifndef __ReceptionCalibrator_H
#define __ReceptionCalibrator_H

#include "Calibrator.h"
#include "Calibration/ReceptionModel.h"
#include "Calibration/Polar.h"
#include "Calibration/FunctionTransformation.h"

namespace Pulsar {

  class Archive;
  class PolnCalibrator;
  class FluxCalibrator;

  //! Uses ReceptionModel to represent and fit for the system response
  /*! The ReceptionCalibrator implements the technique of single dish
    polarimetric self-calibration.  This class requires a number of
    constraints, which are provided in through the ReceptionSet
    class. */

  class ReceptionCalibrator : public Calibrator {
    
  public:

    friend class ReceptionCalibratorPlotter;

    //! Construct from the best estimate of the average pulse profile
    ReceptionCalibrator (const Archive* archive);

    //! Destructor
    ~ReceptionCalibrator ();

    //! Add the specified pulse phase bin to the set of state constraints
    void add_state (float pulse_phase);

    //! Get the number of pulse phase bin state constraints
    unsigned get_nstate () const;

    //! Add the specified pulsar observation to the set of constraints
    void add_observation (const Archive* data);

    //! Add the specified PolnCalibrator observation to the set of constraints
    void add_PolnCalibrator (const PolnCalibrator* polncal);

    //! Add the specified FluxCalibrator observation to the set of constraints
    void add_FluxCalibrator (const FluxCalibrator* fluxcal);

    //! Calibrate the polarization of the given archive
    virtual void calibrate (Archive* archive);

  protected:

    //! Model of receiver and source states as a function of frequency
    vector<Calibration::ReceptionModel> model;

    //! Model of receiver as a function of frequency
    vector<Calibration::Polar> receiver;

    //! Model of backend as a function of frequency and time
    vector<Calibration::FunctionTransformation> backend;

    //! Best, uncalibrated estimate of the average pulse profile
    Reference::To<const Archive> uncalibrated;
  };

}

#endif
