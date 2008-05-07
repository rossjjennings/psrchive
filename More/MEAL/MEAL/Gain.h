//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Gain.h,v $
   $Revision: 1.9 $
   $Date: 2008/05/07 02:33:38 $
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

    //! Set the gain
    void set_gain (const Estimate<double>&);

    //! Get the gain
    Estimate<double> get_gain () const;

    //! Set the name of the parameter
    void set_param_name (const std::string& name);

    //! Set the description of the parameter
    void set_param_description (const std::string& name);

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
