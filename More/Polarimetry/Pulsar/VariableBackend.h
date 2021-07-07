//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/VariableBackend.h

#ifndef __CalibrationVariableBackend_H
#define __CalibrationVariableBackend_H

#include "MEAL/ChainRule.h"
#include "MEAL/Complex2.h"
#include "MEAL/ScalarParameter.h"

#include "Estimate.h"
#include "Stokes.h"

namespace Calibration {

  class SingleAxis;

  //! Physical parameterization of the instrumental response

  /*! Abstract base class of instrumental response parameterizations
   that separate the backend and frontend transformations. */

  class VariableBackend : public MEAL::ChainRule<MEAL::Complex2>
  {

  public:

    //! Default Constructor
    VariableBackend ();

    //! Copy Constructor
    VariableBackend (const VariableBackend& s);

    //! Assignment Operator
    const VariableBackend& operator = (const VariableBackend& s);

    //! Destructor
    ~VariableBackend ();

    //! Clone operator
    VariableBackend* clone () const;
    
    //! Set cyclical bounds on the differential phase
    void set_cyclic (bool flag = true);

    //! Get the instrumental gain, \f$ G \f$, in calibrator flux units
    Estimate<double> get_gain () const;

    //! Get the differential gain, \f$ \gamma \f$, in hyperbolic radians
    Estimate<double> get_diff_gain () const;
    
    //! Get the differential phase, \f$ \phi \f$, in radians
    Estimate<double> get_diff_phase () const;

    //! Set the instrumental gain, \f$ G \f$, in calibrator flux units
    void set_gain (const Estimate<double>& gain);

    //! Set the differential gain, \f$ \gamma \f$, in hyperbolic radians
    void set_diff_gain (const Estimate<double>& gamma);
    
    //! Set the differential phase, \f$ \phi \f$, in radians
    void set_diff_phase (const Estimate<double>& phi);

    //! Provide access to the SingleAxis model
    const SingleAxis* get_backend () const;
    SingleAxis* get_backend ();

    //! Set the instrumental gain variation
    void set_gain_variation (MEAL::Scalar*);

    //! Set the differential gain variation
    void set_diff_gain_variation (MEAL::Scalar*);
    
    //! Set the differential phase variation
    void set_diff_phase_variation (MEAL::Scalar*);

    //! Get the instrumental gain variation
    const MEAL::Scalar* get_gain_variation () const;

    //! Get the differential gain variation
    const MEAL::Scalar* get_diff_gain_variation () const;
    
    //! Get the differential phase variation
    const MEAL::Scalar* get_diff_phase_variation () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! SingleAxis model: \f$G\f$, \f$\gamma\f$, and \f$\varphi\f$
    Reference::To<SingleAxis> backend;

    //! Scalar function used to model gain variation
    Reference::To<MEAL::Scalar> gain_variation;

    //! Scalar function used to model differential gain variation
    Reference::To<MEAL::Scalar> diff_gain_variation;

    //! Scalar function used to model differential phase variation
    Reference::To<MEAL::Scalar> diff_phase_variation;

  private:

    //! Initialize function used by constructors
    void init ();

  };

}

#endif

