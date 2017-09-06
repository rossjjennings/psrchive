//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ReceptionModelSolveMEAL.h

#ifndef __ReceptionModel_SolveMEAL_H
#define __ReceptionModel_SolveMEAL_H

#include "Pulsar/ReceptionModelSolver.h"

namespace Calibration
{
  //! Solve the measurement equation by non-linear least squares minimization
  class SolveMEAL : public ReceptionModel::Solver
  {

    //! Return the name of this solver
    std::string get_name () const;

    //! Return a new, copy-constructed clone
    SolveMEAL* clone () const;

  protected:

    //! Solve the measurement equation using MEAL::LevenbergMarquardt
    void fit ();

  };

}

#endif

