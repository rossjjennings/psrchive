//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/Coherency.h,v $
   $Revision: 1.3 $
   $Date: 2004/11/22 19:26:03 $
   $Author: straten $ */

#ifndef __MEAL_Coherency_H
#define __MEAL_Coherency_H

#include "MEAL/OptimizedComplex2.h"
#include "MEAL/Parameters.h"

#include "Estimate.h"
#include "Stokes.h"

namespace MEAL {

  //! Represents a Hermitian coherency matrix
  /*! This class represents a source in which the four Stokes parameters
    are independent of eachother. */
  class Coherency : public OptimizedComplex2 {

  public:

    //! Default constructor
    Coherency ();

    //! Set the Stokes parameters of the model
    void set_stokes (const Stokes<double>& stokes);
    
    //! Set the Stokes parameters and their estimated errors
    void set_stokes (const Stokes< Estimate<double> >& stokes);

    //! Get the Stokes parameters and their estimated errors
    Stokes< Estimate<double> > get_stokes () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

    //! Return the name of the specified parameter
    std::string get_param_name (unsigned index) const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // OptimizedComplex2 implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calculate the Jones matrix and its gradient
    void calculate (Jones<double>& result, std::vector<Jones<double> >* gradient);

  };

}

#endif
