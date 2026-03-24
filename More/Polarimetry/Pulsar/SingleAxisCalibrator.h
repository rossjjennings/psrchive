//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SingleAxisCalibrator.h

#ifndef __Pulsar_SingleAxisCalibrator_H
#define __Pulsar_SingleAxisCalibrator_H

#include "Pulsar/ReferenceCalibrator.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/SingleAxisSolver.h"

namespace Pulsar {

  //! Represents the system as a Calibration::SingleAxis
  class SingleAxisCalibrator : public ReferenceCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    SingleAxisCalibrator (const Archive* archive);

    //! Copy constructor
    SingleAxisCalibrator (const SingleAxisCalibrator&);

    //! Clone operator
    SingleAxisCalibrator* clone () const;

    //! Destructor
    ~SingleAxisCalibrator ();

    //! Communicates SingleAxisCalibrator parameters
    class Info;

    //! Return the SingleAxisCalibrator information
    Calibrator::Info* get_Info () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::ReferenceCalibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return a new Calibration::SingleAxis instance
    MEAL::Complex2* solve (const std::vector<Estimate<double> >& hi,
			   const std::vector<Estimate<double> >& lo);

    //! Optimization for SingleAxis solve
    Reference::To< Calibration::SingleAxisSolver > solver;

  };

}

#endif
