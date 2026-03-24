//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SystemCalibratorManager.h

#ifndef __Pulsar_SystemCalibratorManager_H
#define __Pulsar_SystemCalibratorManager_H

#include "Pulsar/SystemCalibrator.h"
#include "Pulsar/Database.h"

namespace Pulsar
{
  //! Manages multiple sources and a common model
  /*! Each SystemCalibrator manages input data from a single pulsar.
    This class manages input data from multiple pulsars using multiple
    SystemCalibrator instances that share a common SignalPath (for each
    frequency channel).

    The first SystemCalibrator instance to have pulsar observations (data)
    added successfully added to it becomes the fiducial model that is
    partnered with all other SystemCalibrator instances.
  */
  class SystemCalibratorManager : public Reference::Able
  {

  public:

    //! Default constructor
    SystemCalibratorManager ();

    //! Add to the array of system calibrators
    void manage (SystemCalibrator*);

    //! Return the ith managed system calibrator
    SystemCalibrator* get_calibrator (unsigned i);

    //! Return the number of managed system calibrators
    unsigned get_ncalibrator ();

    //! Return the system calibrator that matches the data
    SystemCalibrator* get_calibrator (const Archive*);

    //! Return the fiducial system calibrator
    SystemCalibrator* get_model ();

    //! Solve the entire system of calibrators
    void solve ();

    //! Prepare the data for inclusion in the model
    virtual void preprocess (Archive* data);

    //! Add the observation to the set of constraints
    virtual void add_observation (const Archive* data);

    //! Pre-calibrate the polarization of the given archive
    virtual void precalibrate (Archive* archive);

    void set_fscrunch_data_to_model (bool f) { fscrunch_data_to_model = f; }
    
  protected:

    //! The system calibrators
    std::vector< Reference::To<SystemCalibrator> > calibrator;

    //! If needed, frequency integrate data to same resolution as model
    bool fscrunch_data_to_model = false;

    //! Delay setting up sharing in case first data does not match first model
    bool sharing_setup = false;

    //! Setup information sharing between system calibrator instances
    /*! \param model candidate fiducial model to be partnered with all other models */
    void setup_sharing (SystemCalibrator* model);

  };

}

#endif

