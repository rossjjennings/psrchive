//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2017 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ReceptionModelSolveCeres.h

#ifndef __ReceptionModel_SolveCeres_H
#define __ReceptionModel_SolveCeres_H

#include "Pulsar/ReceptionModelSolver.h"

namespace Calibration
{
  //! Solve the measurement equation using Google's Ceres Solver
  class SolveCeres : public ReceptionModel::Solver
  {

    //! Return the name of this solver
    std::string get_name () const;

    //! Return a new, copy-constructed clone
    SolveCeres* clone () const;

  protected:

    //! Solve the measurement equation using Ceres Solver
    void fit ();

  };

}

#endif

