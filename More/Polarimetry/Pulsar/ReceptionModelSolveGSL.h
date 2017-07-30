//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ReceptionModelSolveGSL.h

#ifndef __ReceptionModel_SolveGSL_H
#define __ReceptionModel_SolveGSL_H

#include "Pulsar/ReceptionModelSolver.h"

namespace Calibration
{
  //! Solve the measurement equation using GNU Scientific Library
  class SolveGSL : public ReceptionModel::Solver
  {

    //! Return the name of this solver
    std::string get_name () const;

    //! Return a new, copy-constructed clone
    SolveGSL* clone () const;

  protected:

    //! Solve the measurement equation using GSL
    void fit ();

  };

}

#endif

