//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SystemCalibrator.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
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
    virtual CalibratorStokes* get_CalibratorStokes () const;

    //! Return a Calibrator::new_solution with a CalibratorStokesExtension
    virtual Archive* new_solution (const std::string& archive_class) const;

  protected:

    //! The CalibratorStokesExtension of the Archive passed during construction
    Reference::To<CalibratorStokes> calibrator_stokes;

  };

}

#endif

