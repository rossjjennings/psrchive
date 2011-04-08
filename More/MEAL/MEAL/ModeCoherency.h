//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/ModeCoherency.h

#ifndef __MEAL_ModeCoherency_H
#define __MEAL_ModeCoherency_H

#include "MEAL/Coherency.h"
#include "MEAL/Wrap.h"
#include "Vector.h"

namespace MEAL {

  class Scalar;
  class ScalarParameter;
  class BoostUnion;
  class UnitTangent;

  //! A coherency matrix parameterized by a logarithmic gain times a boost
  /*! This class ensures that the result is a positive definite Hermitian
    Hermitian matrix by parameterizing the four degrees of freedom as:
   <ol>
   <li> log(I), where I is the total intensity
   <li> log(beta), where beta = atanh(p) and p is the degree of polarization
   <li> unit 3-vector, so that orthogonal mode may be well-defined
  */
  class ModeCoherency : public Wrap<Coherency>
  {

  public:

    //! Default constructor
    ModeCoherency ();

    //! Copy constructor
    ModeCoherency (const ModeCoherency& copy);

    //! Assignment operator
    ModeCoherency& operator = (const ModeCoherency& copy);

    //! Destructor
    ~ModeCoherency ();

    //! Get the model of the unit vector
    UnitTangent* get_axis ();

    //! Set the model of the unit vector
    void set_axis (UnitTangent*);

    //! Get beta
    Scalar* get_beta ();

    //! Get the intensity
    Scalar* get_intensity ();

    //! Set the Stokes parameters of the model
    virtual void set_stokes (const Stokes<double>& stokes);
    
    //! Set the Stokes parameters and their estimated errors
    virtual void set_stokes (const Stokes< Estimate<double> >& stokes);

    //! Get the Stokes parameters and their estimated errors
    virtual Stokes< Estimate<double> > get_stokes () const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Works for the constructors
    void init ();

    Reference::To<BoostUnion> boost;
    Reference::To<UnitTangent> axis;
    Reference::To<ScalarParameter> log_beta;
    Reference::To<ScalarParameter> log_intensity;

    Reference::To<Scalar> beta;
    Reference::To<Scalar> intensity;
  };

}

#endif
