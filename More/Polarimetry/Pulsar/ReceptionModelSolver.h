//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ReceptionModelSolver.h

#ifndef __ReceptionModelSolver_H
#define __ReceptionModelSolver_H

#include "Pulsar/ReceptionModel.h"
#include "MEAL/LeastSquares.h"
#include <fstream>

namespace Calibration
{
  //! Solve the measurement equation by non-linear least squares minimization
  class ReceptionModel::Solver : public MEAL::LeastSquares
  {

  public:

    Solver () { verbose = 0; }
    
    //! report the reduced chisq on completion
    static bool report_chisq;

    //! Return a new, copy-constructed clone
    virtual Solver* clone () const = 0;

    //! Set the measurement equation to be solved
    void set_equation (ReceptionModel*);

    //! Add a convergence condition
    void add_convergence_condition( Functor< bool(ReceptionModel*) > );

    //! Add an acceptance condition
    void add_acceptance_condition( Functor< bool(ReceptionModel*) > );

    //! Solve the measurement equations by least squares minimization
    virtual void solve ();

    //! Set the verbosity level (0 = quiet, 3 = most verbose)
    void set_verbosity (unsigned level) { verbose = level; }
    unsigned get_verbosity () const { return verbose; }
    
  protected:

    //! The fit is performed by derived classes
    virtual void fit () = 0;

    //! Additional convergence conditions
    std::vector< Functor< bool(ReceptionModel*) > > convergence_condition;

    //! Additional acceptance conditions
    std::vector< Functor< bool(ReceptionModel*) > > acceptance_condition;

    //! Filename to which a pre-fit report will be written
    std::string prefit_report_filename;

    //! Count the number of parameters to be fit
    void count_infit ();

    //! Count the total number of constraints (one-dimensional data)
    void count_constraint ();

    void check_constraints ();
    void set_variances ();
    void check_solution ();

    //! The measurement equation to be solved
    Reference::To<ReceptionModel, false> equation;

    //! The observations used to constrain the measurement equations
    std::vector<CoherencyMeasurementSet>& get_data ();

    //! Verbosity level
    unsigned verbose;
    
 private:

    //! Flags set true if state has been observed
    std::vector<bool> state_observed;

    //! Flags set true if path has been observed
    std::vector<bool> path_observed;
  };

}

#endif

