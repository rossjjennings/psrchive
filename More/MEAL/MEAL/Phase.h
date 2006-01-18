//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Phase.h,v $
   $Revision: 1.5 $
   $Date: 2006/01/18 23:22:19 $
   $Author: straten $ */

#ifndef __MEAL_Phase_H
#define __MEAL_Phase_H

#include "MEAL/Complex2.h"

namespace MEAL {

  //! A gain transformation
  class Phase : public Complex2 {

  public:

    //! Default constructor
    Phase ();

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
   
  };

}

#endif
