//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReceptionCalibrator.h,v $
   $Revision: 1.3 $
   $Date: 2003/04/07 10:02:02 $
   $Author: straten $ */

#ifndef __ReceptionCalibrator_H
#define __ReceptionCalibrator_H

#include "Calibrator.h"
#include "Calibration/ReceptionModel.h"

namespace Pulsar {

  //! Uses ReceptionModel to represent and fit for the system response
  /*! The ReceptionCalibrator implements the technique of single dish
    polarimetric self-calibration.  This class requires a number of
    constraints, which are provided in through the ReceptionSet
    class. */

  class ReceptionCalibrator : public Calibrator {
    
  public:

    friend class ReceptionCalibratorPlotter;

    //! Constructor
    ReceptionCalibrator ();

    //! Destructor
    ~ReceptionCalibrator ();

    //! Set the number of source polarization states for which to solve
    void set_nsource (unsigned nsource);
    //! Get the number of source polarization states for which to solve
    unsigned get_nsource () const;

    //! Set the number of calibrator polarization states for which to solve
    void set_ncalibrator (unsigned ncalibrator);
    //! Get the number of calibrator polarization states for which to solve
    unsigned get_ncalibrator () const;

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Calibrate the polarization of the given archive
    virtual void calibrate (Archive* archive);

  protected:

    //! Model of receiver and source states as a function of frequency
    vector<Calibration::ReceptionModel> model;

    //! Number of frequency channels
    unsigned nchan;

    //! Number of source polarization states
    unsigned nsource;

    //! Number of calibrator polarization states
    unsigned ncalibrator;

    //! Resizes the model and data arrays
    void size_dataspace ();

  };

}

#endif
