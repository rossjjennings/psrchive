//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ReceptionModel.h

#ifndef __ReceptionModel_H
#define __ReceptionModel_H

#include "Pulsar/MeasurementEquation.h"
#include "MEAL/SumRule.h"
#include "Estimate.h"

namespace Calibration {

  class CoherencyMeasurementSet;

  //! Models a set of transformations and source polarizations
  /*! This class models the output polarization of an arbitrary number of
    both input polarization states and signal paths.  Each signal path
    is represented by a MeasurementEquation. */

  class ReceptionModel : public MeasurementEquation
  {

  public:

    //! null constructor
    ReceptionModel ();

    //! destructor
    ~ReceptionModel ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Add data to the model
    //
    // ///////////////////////////////////////////////////////////////////

    //! Checks that each CoherencyMeasurement has a valid source_index
    virtual void add_data (CoherencyMeasurementSet& data);

    //! Get the number of CoherencyMeasurementSet
    unsigned get_ndata () const;

    //! Get the specified CoherencyMeasurementSet
    const CoherencyMeasurementSet& get_data (unsigned idata) const;

    //! Delete all data
    void delete_data ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Report on the model
    //
    // ///////////////////////////////////////////////////////////////////

    class Report;

    //! Add a report to be executed before solving the measurement equation
    void add_prefit_report (Report*);

    //! Add a report to be executed after solving the measurement equation
    void add_postfit_report (Report*);

    // ///////////////////////////////////////////////////////////////////
    //
    // Fit the model
    //
    // ///////////////////////////////////////////////////////////////////

    class Solver;

    //! Set the algorithm used to solve the measurement equation
    void set_solver (Solver*);

    //! Get the algorithm used to solve the measurement equation
    Solver* get_solver ();
    const Solver* get_solver () const;

    //! Solve the measurement equation using the current algorithm
    void solve ();

    //! Return true when solved
    bool get_solved () const;

    //! Copy the fitted parameters from another model
    void copy_fit (const ReceptionModel*);

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Ensure that idata <= get_ndata()
    void range_check (unsigned idata, const char* method) const;

    //! The algorithm used to solve the measurement equation
    Reference::To<Solver> solver;

    //! Reports to run before solving
    std::vector< Reference::To<Report> > prefit_reports;

    //! Reports to run after solving
    std::vector< Reference::To<Report> > postfit_reports;

 private:

    //! Provide Solver class with acces to data
    friend class Solver;

    //! Observations used to constrain the measurement equations
    std::vector<CoherencyMeasurementSet> data;

  };


}

#endif

