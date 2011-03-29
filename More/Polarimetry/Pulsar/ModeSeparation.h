//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ModeSeparation.h,v $
   $Revision: 1.2 $
   $Date: 2008/05/28 08:32:34 $
   $Author: straten $ */

#ifndef __Pulsar_ModeSeparation_H
#define __Pulsar_ModeSeparation_H

#include "MEAL/Real4.h"
#include "MEAL/Coherency.h"
#include "MEAL/ScalarVector.h"
#include "MEAL/ScalarParameter.h"

namespace Pulsar {

  //! Separates polarized modes of emission
  class ModeSeparation : public Reference::Able
  {
  public:

    //! Default constructor
    ModeSeparation ();

  protected:

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
    Reference::To<MEAL::Coherency> mode_C;
    Reference::To<MEAL::ScalarParameter> dof_C;

    //! Vector space of problem
    Reference::To<MEAL::ScalarVector> space;

    void init ();

  };

}

#endif

