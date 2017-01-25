//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/SpinorJones.h

#ifndef __MEAL_SpinorJones_H
#define __MEAL_SpinorJones_H

#include "MEAL/Complex2.h"
#include "MEAL/Spinor.h"
#include "MEAL/Composite.h"

namespace MEAL {

  //! Returns the outer product of two spinors, \f$ A B^\dagger \f$

  class SpinorJones : public MEAL::Complex2
  {

  public:

    //! Default constructor
    SpinorJones ();

    //! Set spinor A == spinor B
    virtual void set_spinor (Spinor*);

    //! Set spinor A
    virtual void set_spinorA (Spinor*);

    //! Get spinor A
    virtual Spinor* get_spinorA () { return spinorA; }

    //! Set spinor B
    virtual void set_spinorB (Spinor*);

    //! Get spinor B
    virtual Spinor* get_spinorB () { return spinorB; }

    std::string get_name () const;

  protected:

    //! Calculate the Mueller matrix and its gradient
    virtual void calculate (Jones<double>& result,
			    std::vector< Jones<double> >*);

    //! Spinor A
    Project<Spinor> spinorA;

    //! Spinor B
    Project<Spinor> spinorB;

    //! Composite parameter policy
    Composite composite;

    //! Does the work for set_spinorA and set_spinorB
    void set_spinor (Project<Spinor>&, Spinor*);

  };

}

#endif

