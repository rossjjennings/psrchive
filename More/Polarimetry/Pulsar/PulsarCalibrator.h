//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PulsarCalibrator.h,v $
   $Revision: 1.4 $
   $Date: 2004/04/20 13:07:36 $
   $Author: straten $ */

#ifndef __Pulsar_PulsarCalibrator_H
#define __Pulsar_PulsarCalibrator_H

#include "Pulsar/PolnCalibrator.h"

#include "Calibration/Parallactic.h"
#include "Calibration/MeanModel.h"

namespace Pulsar {

  class Archive;
  class PolnProfileFit;
  class ArtificialCalibrator;

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
    Calibrator::Info* get_Info () const;

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Set the standard to which pulsar profiles will be fit
    void set_standard (const Archive* data);

    //! Add the observation to the set of constraints
    void add_observation (const Archive* data);
    
    //! Add the calibrator observation to the set of constraints
    void add_calibrator (const Archive* data);
    
    //! Add the ArtificialCalibrator observation to the set of constraints
    void add_calibrator (const ArtificialCalibrator* polncal);

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

    //! The parallactic angle rotation
    Calibration::Parallactic parallactic;

    //! The model specified on construction
    Calibrator::Type model_type;

    //! The maximum number of harmonics to include in the fit
    unsigned maximum_harmonic;

  };

}

#endif
