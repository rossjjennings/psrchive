//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Univariate.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/22 16:00:09 $
   $Author: straten $ */

#ifndef __Model_Univariate_H
#define __Model_Univariate_H

#include "MEPL/ArgumentBehaviour.h"

namespace Model {

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
