//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/PolarCalibrator.h

#ifndef __PolarCalibrator_H
#define __PolarCalibrator_H

#include "Pulsar/ReferenceCalibrator.h"
#include "MEAL/Polar.h"

namespace Pulsar {

  //! Represents the system as a polar decomposition
  class PolarCalibrator : public ReferenceCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    PolarCalibrator (const Archive* archive);

    //! Copy constructor
    PolarCalibrator (const PolarCalibrator&);

    //! Clone operator
    PolarCalibrator* clone () const;

    //! Destructor
    ~PolarCalibrator ();

    //! Communicates PolarCalibrator parameters to plotting routines
    class Info;

    //! Return the PolarAxisCalibrator information
    Calibrator::Info* get_Info () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::ReferenceCalibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return a new Calibration::Polar instance
    MEAL::Complex2* solve (const std::vector<Estimate<double> >& hi,
                           const std::vector<Estimate<double> >& lo);

  };

}

#endif
