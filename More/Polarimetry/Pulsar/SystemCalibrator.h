//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SystemCalibrator.h,v $
   $Revision: 1.1 $
   $Date: 2003/12/31 17:42:49 $
   $Author: straten $ */

#ifndef __Pulsar_SystemCalibrator_H
#define __Pulsar_SystemCalibrator_H

#include "Pulsar/PolnCalibrator.h"

namespace Pulsar {

  class Integration;
  class CalibratorStokes;

  //! PolnCalibrator with estimated calibrator Stokes parameters
  /*! The SystemCalibrator is the virtual base class of polarization
    calibrators that determine both the instrumental response and the
    input Stokes parameters of the reference signal.
  */
  class SystemCalibrator : public PolnCalibrator {

  public:

    //! Construct with optional processed calibrator Archive
    SystemCalibrator (Archive* archive = 0);

    //! Copy constructor
    SystemCalibrator (const SystemCalibrator& calibrator);

    //! Destructor
    virtual ~SystemCalibrator ();

    //! Return the CalibratorStokesExtension
    virtual CalibratorStokes* get_calibrator_stokes () const;

    //! Return a PolnCalibrator::get_solution with a CalibratorStokesExtension
    virtual Archive* get_solution (const string& archive_class,
				   string filename_extension = ".pcm") const;

  protected:

    //! The CalibratorStokesExtension of the Archive passed during construction
    Reference::To<CalibratorStokes> calibrator_stokes;

  };

}

#endif

