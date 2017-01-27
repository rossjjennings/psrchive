//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ModeSeparation.h

#ifndef __Pulsar_ModeSeparation_H
#define __Pulsar_ModeSeparation_H

#include "MEAL/Real4.h"
#include "MEAL/CrossCoherency.h"
#include "MEAL/ComplexCorrelation.h"
#include "MEAL/ScalarVector.h"
#include "MEAL/ScalarParameter.h"

namespace Pulsar {

  //! Separates polarized modes of emission
  class ModeSeparation : public Reference::Able
  {
  public:

    static bool verbose;

    //! Default constructor
    ModeSeparation ();

    //! Set the estimated mean Stokes parameters
    void set_mean (const Stokes< Estimate<double> >&);

    //! Set the estimated covariance matrix of the Stokes parameters
    void set_covariance (const Matrix<4,4,Estimate<double> >&);

    //! Solve the equation
    void solve ();

    MEAL::Coherency* get_modeA () { return mode_A; }
    MEAL::Coherency* get_modeB () { return mode_B; }
    MEAL::Coherency* get_modeC () { return mode_C; }
    MEAL::Complex* get_correlation () { return correlation; }
    MEAL::Complex2* get_mean () { return mean; }
    MEAL::Real4* get_covariance () { return covariance; }

  protected:

    //! Mean Stokes parameters
    Stokes< Estimate<double> > obs_mean;
    
    //! Mean covariance matrix
    Matrix< 4,4,Estimate<double> > obs_covariance;

    //! Covariance of the mode sum
    Reference::To<MEAL::Real4> covariance;

    //! Mean value of the mode sum
    Reference::To<MEAL::Complex2> mean;

    //! Mode A
    Reference::To<MEAL::Coherency> mode_A;
    Reference::To<MEAL::ScalarParameter> dof_A;

    //! Mode B
    Reference::To<MEAL::Coherency> mode_B;
    Reference::To<MEAL::ScalarParameter> dof_B;

    //! Mode C (cross-coherency)
    Reference::To<MEAL::CrossCoherency> cross;
    Reference::To<MEAL::ComplexCorrelation> correlation;

    Reference::To<MEAL::Coherency> mode_C;
    Reference::To<MEAL::ScalarParameter> dof_C;

    //! Vector space of problem
    Reference::To<MEAL::ScalarVector> space;

    void init ();

  };

}

#endif

