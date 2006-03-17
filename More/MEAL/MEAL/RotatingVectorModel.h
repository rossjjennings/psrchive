//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/RotatingVectorModel.h,v $
   $Revision: 1.4 $
   $Date: 2006/03/17 13:35:26 $
   $Author: straten $ */

#ifndef __RotatingVectorModel_H
#define __RotatingVectorModel_H

#include "MEAL/Scalar.h"
#include "MEAL/Univariate.h"

namespace MEAL {

  class ScalarParameter;

  //! The Rotating Vector Model (RVM) of Radhakrishnan & Cooke
  /*! The abscissa to this model is the pulsar rotational phase in radians */
  class RotatingVectorModel : public Univariate<Scalar>
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

    //! Set the reference position angle
    void set_reference_position_angle (const Estimate<double>& position_angle);

    //! Get the reference position angle
    Estimate<double> get_reference_position_angle () const;

    //! Set the latitude of the line of sight
    void set_line_of_sight (const Estimate<double>& line_of_sight);

    //! Get the latitude of the line of sight
    Estimate<double> get_line_of_sight () const;

    //! Set the latitude of the magnetic axis
    void set_magnetic_axis (const Estimate<double>& magnetic_axis);

    //! Get the latitude of the magnetic axis
    Estimate<double> get_magnetic_axis () const;

    //! Set the longitude of the magnetic meridian
    void set_magnetic_meridian (const Estimate<double>& magnetic_meridian);

    //! Get the longitude of the magnetic meridian
    Estimate<double> get_magnetic_meridian () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  private:

    ScalarParameter* reference_position_angle;
    ScalarParameter* line_of_sight;
    ScalarParameter* magnetic_axis;
    ScalarParameter* magnetic_meridian;

    void calculate (double&, std::vector<double>*) {}
    void init ();

    Reference::To<Scalar> expression;

  };

}

#endif
