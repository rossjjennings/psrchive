//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/RVM.h

#ifndef __RVM_H
#define __RVM_H

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"
#include "MEAL/ScalarParameter.h"

namespace MEAL {

  //! Base class of different Rotating Vector Model parameterizations
  /*! The abscissa to this model is the pulsar rotational phase in radians */
  class RVM : public Univariate<Scalar>
  {

  public:

    //! psi0: position angle at magnetic meridian
    /*! effectively rotation of pulsar about the line of sight */
    Reference::To<ScalarParameter> reference_position_angle;

    //! phi0: longitude of the magnetic merdian
    Reference::To<ScalarParameter> magnetic_meridian;

    //! Get the projection onto North
    Scalar* get_north () { return north; }
    //! Get the projection onto East
    Scalar* get_east () { return east; }

  protected:
    
    // the magnetic field line projected onto North
    Reference::To<Scalar> north;

    // the magnetic field line projected onto East
    Reference::To<Scalar> east;

  private:

    // disable the calculate method
    /* derived classes copy the evaluation policy of the
       resulting ScalarMath expression */
    void calculate (double&, std::vector<double>*) {}

  };

}

#endif
