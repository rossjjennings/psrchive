//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/RotatingVectorModel.h

#ifndef __RotatingVectorModel_H
#define __RotatingVectorModel_H

#include "MEAL/RVM.h"
#include "MEAL/SumRule.h"

namespace MEAL {

  class ScalarParameter;

  //! The Rotating Vector Model (RVM) of Radhakrishnan & Cooke
  /*! The abscissa to this model is the pulsar rotational phase in radians */
  class RotatingVectorModel : public RVM
  {

  public:

    //! Default constructor
    RotatingVectorModel ();

    //! Copy constructor
    RotatingVectorModel (const RotatingVectorModel& copy);

    //! Assignment operator
    RotatingVectorModel& operator = (const RotatingVectorModel& copy);

    //! Destructor
    ~RotatingVectorModel ();

    //! zeta: colatitude of line of sight with respect to spin axis
    Reference::To<ScalarParameter> line_of_sight;

    //! alpha: colatitude of magnetic axis with respect to spin axis
    Reference::To<ScalarParameter> magnetic_axis;

    //! beta: colatitude of line of sight with respect to magnetic axis
    Reference::To<ScalarParameter> impact;

    //! Switch to using impact as a free parameter
    void use_impact (bool flag = true);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  private:

    void calculate (double&, std::vector<double>*) {}
    void init ();

    // the sum rule used to switch between zeta and beta
    Reference::To< SumRule<Scalar> > zeta_sum;

  };

}

#endif
