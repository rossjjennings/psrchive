//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Univariate.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:04 $
   $Author: straten $ */

#ifndef __MEAL_Univariate_H
#define __MEAL_Univariate_H

#include "MEAL/ArgumentBehaviour.h"

namespace MEAL {

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
