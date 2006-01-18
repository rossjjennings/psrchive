//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Gain.h,v $
   $Revision: 1.5 $
   $Date: 2006/01/18 23:22:19 $
   $Author: straten $ */

#ifndef __MEAL_Gain_H
#define __MEAL_Gain_H

#include "MEAL/Complex2.h"

namespace MEAL {

  //! A gain transformation
  class Gain : public Complex2 {

  public:

    //! Default constructor
    Gain ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector< Jones<double> >*);
   
  };

}

#endif
