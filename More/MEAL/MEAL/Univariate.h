//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Univariate.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:17:17 $
   $Author: straten $ */

#ifndef __Calibration_Univariate_H
#define __Calibration_Univariate_H

#include "Calibration/ArgumentBehaviour.h"

namespace Calibration {

  //! A function of one scalar variable
  class Univariate : public ArgumentBehaviour {

  public:

    //! Default constructor
    Univariate ();

    //! Destructor
    ~Univariate ();

    //! Copy constructor
    Univariate (const Univariate&);

    //! Assignment operator
    const Univariate& operator = (const Univariate&);

    //! Set the abscissa value
    void set_abscissa (double value);

    //! Get the abscissa value
    double get_abscissa () const;

    //! Connect the set_abscissa method to the axis value
    void set_argument (unsigned dimension, Argument* axis);

  protected:

    //! The abscissa value
    double abscissa;

  };

}

#endif
