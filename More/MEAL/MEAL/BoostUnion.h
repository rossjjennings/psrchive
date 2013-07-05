//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/BoostUnion.h

#ifndef __MEAL_BoostUnion_H
#define __MEAL_BoostUnion_H

#include "MEAL/Complex2.h"
#include "MEAL/Composite.h"

#include "Vector.h"

namespace MEAL {

  //! Represents a boost (Hermitian, dichroic) transformation
  /*! This class represents the boost, \f$\beta\f$, along an 
    axis, \f$\hat m\f$, defined by a unit vector model. */
  class BoostUnion : public Complex2 {

  public:

    BoostUnion ();

    //! Set the model that defines the unit vector
    void set_axis (Evaluable< Vector<3,double> >*);

    //! Set the model that defines the boost parameter
    void set_beta (Evaluable<double>*);

    //! Get the unit-vector along which the boost occurs
    Vector<3, double> get_axis () const;

    //! Get the boost parameter, beta
    double get_beta () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >*);

    //! The model that defines the axis
    Project< Evaluable< Vector<3,double> > > axis;

    //! The boost parameter
    Project< Evaluable<double> > beta;

    //! Composite parameter policy
    Composite composite;

  };

}

#endif
