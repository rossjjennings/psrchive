//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PulsarCalibrator.h,v $
   $Revision: 1.6 $
   $Date: 2004/07/12 10:54:01 $
   $Author: straten $ */

#ifndef __Pulsar_PulsarCalibrator_H
#define __Pulsar_PulsarCalibrator_H

#include "Pulsar/PolnCalibrator.h"

#include "Calibration/Complex2Constant.h"
#include "Calibration/MeanModel.h"

namespace Pulsar {

  class Archive;
  class PolnProfileFit;
  class ReferenceCalibrator;

  //! Uses PolnProfileFit to determine the system response
  /*! The PulsarCalibrator implements a technique of polarimetric
    calibration using a well-determined source.  This class requires a
    polarimetric standard and another observation of the same source.  */
  class PulsarCalibrator : public PolnCalibrator {
    
  public:
    
    //! Constructor
    PulsarCalibrator (Calibrator::Type model);

    //! Destructor
    ~PulsarCalibrator ();

    //! Return the reference epoch of the calibration experiment
    MJD get_epoch () const;

    //! Return Calibrator::Hamaker or Calibrator::Britton
    Type get_type () const;

    //! Return the Calibrator information
    Info* get_Info () const;

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Set the standard to which pulsar profiles will be fit
    void set_standard (const Archive* data);

    //! Add the observation to the set of constraints
    void add_observation (const Archive* data);
    
    //! Set the flag to return the mean solution or the last fit
    void set_return_mean_solution (bool return_mean = true);

    //! Set the solution to the mean
    void update_solution ();

  protected:
    
    //! Initialize the PolnCalibration::transformation attribute
    virtual void calculate_transformation ();

    //! The calibration model as a function of frequency
    vector< Reference::To<PolnProfileFit> > model;

    //! The array of transformation Model instances
    // vector< Reference::To<Calibration::Complex2> > transformation;

    typedef Calibration::MeanModel< Calibration::Complex2 > MeanXform;

    //! The array of transformation Model instances
    vector< Reference::To<MeanXform> > solution;

    //! The known instrumental corrections
    Calibration::Complex2Constant corrections;

    //! The model specified on construction
    Calibrator::Type model_type;

    //! The maximum number of harmonics to include in the fit
    unsigned maximum_harmonic;

    //! When true, calculate_transformation will return the mean solution
    bool mean_solution;

    //! Count of the number of Integrations added
    unsigned integrations_added;

  };

}

#endif
