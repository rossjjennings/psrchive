//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/SingularCoherency.h

#ifndef __MEAL_SingularCoherency_H
#define __MEAL_SingularCoherency_H

#include "MEAL/Coherency.h"

namespace MEAL {

  //! A coherency matrix with determinant equal to zero
  /*! This class represents a 100% polarized state, parameterized by
    three free parameters:
   <ol>
   <li> Stokes Q
   <li> Stokes U
   <li> Stokes V
   </ol>
  */
  class SingularCoherency : public Coherency
  {

  public:

    //! Default constructor
    SingularCoherency ();

    //! Set the Stokes parameters of the model
    virtual void set_stokes (const Stokes<double>& stokes);
    
    //! Set the Stokes parameters and their estimated errors
    virtual void set_stokes (const Stokes< Estimate<double> >& stokes);

    //! Get the Stokes parameters and their estimated errors
    virtual Stokes< Estimate<double> > get_stokes () const;

    //! Set the Stokes polarization vector
    virtual void set_vector (const Vector<3,double>&);

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
