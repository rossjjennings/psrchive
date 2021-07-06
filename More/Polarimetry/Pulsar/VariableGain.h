//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/VariableGain.h

#ifndef __CalibrationVariableGain_H
#define __CalibrationVariableGain_H

#include "MEAL/Gain.h"
#include "MEAL/ChainRule.h"
#include "MEAL/Complex2.h"
#include "MEAL/ScalarParameter.h"

#include "Estimate.h"

namespace Calibration {

  //! Physical parameterization of the instrumental response

  /*! Abstract base class of instrumental response parameterizations
   that separate the backend and frontend transformations. */

  class VariableGain : public MEAL::ChainRule<MEAL::Complex2>
  {

  public:

    //! Default Constructor
    VariableGain ();

    //! Copy Constructor
    VariableGain (const VariableGain& s);

    //! Assignment Operator
    const VariableGain& operator = (const VariableGain& s);

    //! Destructor
    ~VariableGain ();

    //! Get the instrumental gain, \f$ G \f$, in calibrator flux units
    Estimate<double> get_gain () const;

    //! Set the instrumental gain, \f$ G \f$, in calibrator flux units
    void set_gain (const Estimate<double>& gain);

    //! Set the instrumental gain variation
    void set_gain_variation (MEAL::Scalar*);

    //! Get the instrumental gain variation
    const MEAL::Scalar* get_gain_variation () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! gain
    Reference::To< MEAL::Gain<MEAL::Complex2> > gain;

    //! Scalar function used to model gain variation
    Reference::To<MEAL::Scalar> gain_variation;

  private:

    //! Initialize function used by constructors
    void init ();

  };

}

#endif

