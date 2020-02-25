//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/Coherency.h

#ifndef __MEAL_Coherency_H
#define __MEAL_Coherency_H

#include "MEAL/Complex2.h"
#include "Estimate.h"
#include "Stokes.h"

namespace MEAL {

  //! Represents a Hermitian coherency matrix
  /*! This class represents a source in which the four Stokes parameters
    are independent of eachother. */
  class Coherency : public Complex2 {

  public:

    //! Default constructor
    Coherency ();

    //! Set the Stokes parameters of the model
    virtual void set_stokes (const Stokes<double>& stokes);
    
    //! Set the Stokes parameters and their estimated errors
    virtual void set_stokes (const Stokes< Estimate<double> >& stokes);

    //! Get the Stokes parameters and their estimated errors
    virtual Stokes< Estimate<double> > get_stokes () const;

    //! Set the prefix to each parameter name
    void set_param_name_prefix (const std::string& prefix);
    const std::string& get_param_name_prefix () const 
    { return param_name_prefix; }

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

    //! Prefix added to all parameter names
    std::string param_name_prefix;
  };

}

#endif
