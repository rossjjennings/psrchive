//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/MEAL/MEAL/PhaseGradients.h

#ifndef __MEAL_PhaseGradients_H
#define __MEAL_PhaseGradients_H

#include "MEAL/Univariate.h"
#include "MEAL/Complex2.h"
#include "MEAL/Parameters.h"

namespace MEAL {

  //! Multiple phase gradients parameterized by their slopes

  /*! When performing Measurement Equation Template Matching (METM),
    multiple observed pulse profiles are fit to a single well-calibrated
    standard template profile.  In this case, there are two options:

    1. Assume that the ephemeris is good and model all observations using
    a single phase offset between them and the standard profile; in this case,
    this class stores and is parameterized by a single gradient (and offset).

    WARNING: A single offset also assumes that all observations also have
    the same number of phase bins.  This should be checked!

    2. Model each observation using an unique phase offset between the
    observaiton and the standard profile; in this case, this class
    stores and is parameterized by multiple gradients (and offsets).

    This class is a template so that it can be used as a Jones matrix
    (Complex 2) or as a complex-valued scalar (Complex).
  */
  template<typename T>
  class PhaseGradients : public Univariate<T>
  {

  public:

    //! Default constructor
    PhaseGradients (unsigned ngradient = 0);

    //! Copy constructor
    PhaseGradients (const PhaseGradients&);

    //! Assignment operator
    PhaseGradients& operator = (const PhaseGradients&);

    //! Clone operator
    PhaseGradients* clone () const;

    //! Get the number of gradients
    unsigned get_ngradient () const;

    //! Set the current phase gradient index
    void set_igradient (unsigned igradient);

    //! Get the current phase gradient index
    unsigned get_igradient () const;

    //! Set the current phase gradient index
    void set_offset (unsigned igradient, double offset);

    //! Get the current phase gradient index
    double get_offset (unsigned igradient) const;

    //! Add another gradient to the set
    void add_gradient ();

    //! Remove the last gradient from the set
    void remove_gradient ();

    //! Set the number of gradients
    void resize (unsigned ngradient);

    // ///////////////////////////////////////////////////////////////////
    //
    // Function implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    std::string get_name () const;

  protected:

    //! Calculate the Jones matrix and its gradient
    void calculate (typename T::Result& result,
		    std::vector<typename T::Result>* gradient);
   
    //! Parameter policy
    Parameters parameters;

    //! The phase offsets
    std::vector<double> offsets;

    //! The current phase gradient
    unsigned igradient;

  };

}


template<typename T>
MEAL::PhaseGradients<T>::PhaseGradients (unsigned ncoef)
  : parameters (this)
{
  resize (ncoef);
  igradient = 0;
}

//! Copy constructor
template<typename T>
MEAL::PhaseGradients<T>::PhaseGradients (const PhaseGradients& copy)
  : parameters (this)
{
  operator = (copy);
}

//! Assignment operator
template<typename T>
MEAL::PhaseGradients<T>& 
MEAL::PhaseGradients<T>::operator = (const PhaseGradients& copy)
{
  if (&copy == this)
    return *this;

  parameters = copy.parameters;
  offsets = copy.offsets;
  igradient = copy.igradient;

  this->set_evaluation_changed();

  return *this;
}

//! Clone operator
template<typename T>
MEAL::PhaseGradients<T>* MEAL::PhaseGradients<T>::clone () const
{
  return new PhaseGradients( *this );
}

//! Set the current phase gradient
template<typename T>
void MEAL::PhaseGradients<T>::set_igradient (unsigned i)
{
  if (i == igradient)
    return;

  if (i >= get_ngradient())
    throw Error (InvalidParam, "MEAL::PhaseGradients<T>::set_igradient",
		 "invalid index=%d >= ngradient=%d", i, get_ngradient());

  igradient = i;
  this->set_evaluation_changed ();
}

//! Get the current phase gradient index
template<typename T>
unsigned MEAL::PhaseGradients<T>::get_igradient () const
{
  return igradient;
}

//! Set the current phase gradient index
template<typename T>
void MEAL::PhaseGradients<T>::set_offset (unsigned igradient, double offset)
{
#if 0
  cerr << "MEAL::PhaseGradients<T>::set_offset"
    " i=" << igradient << " offset=" << offset << endl;
#endif
  offsets.at(igradient) = offset;
}

//! Get the current phase gradient index
template<typename T>
double MEAL::PhaseGradients<T>::get_offset (unsigned igradient) const
{
  return offsets.at(igradient);
}

//! Set the number of gradients
template<typename T>
void MEAL::PhaseGradients<T>::resize (unsigned ngradient)
{
  unsigned current = get_ngradient();

  if (current == ngradient)
    return;

  parameters.resize( ngradient );
  offsets.resize (ngradient);

  for (unsigned i=current; i<ngradient; i++)
  {
    parameters.set_name (i, "phi_" + tostring(i));
    offsets[i] = 0.0;
  }

  if (ngradient)
    set_igradient (ngradient-1);
}

//! Add another gradient to the set
template<typename T>
void MEAL::PhaseGradients<T>::add_gradient ()
{
  resize ( get_ngradient() + 1 );
}

//! Add another gradient to the set
template<typename T>
void MEAL::PhaseGradients<T>::remove_gradient ()
{
  resize ( get_ngradient() - 1 );
}

//! Get the number of gradients
template<typename T>
unsigned MEAL::PhaseGradients<T>::get_ngradient () const
{
  return parameters.get_nparam();
}

//! Return the name of the class
template<typename T>
std::string MEAL::PhaseGradients<T>::get_name () const
{
  return "PhaseGradients";
}

//! Calculate the Jones matrix and its gradient, as parameterized by gain
template<typename T>
void MEAL::PhaseGradients<T>::calculate (typename T::Result& result,
					 std::vector<typename T::Result>* grad)
{
  double x = this->get_abscissa();

  double phase = (this->get_param(igradient) + this->get_offset(igradient)) * x;

  if (this->verbose)
    std::cerr << "MEAL::PhaseGradients<T>::calculate phase=" << phase
	      << std::endl;

  double cos_phase = ::cos(phase);
  double sin_phase = ::sin(phase);

  result = std::complex<double>(cos_phase, sin_phase);

  if (grad)
  {
    for (unsigned i=0; i<grad->size(); i++)
      (*grad)[i] = 0.0;
    (*grad)[igradient] = x * std::complex<double>(-sin_phase, cos_phase);
    
    if (this->verbose)
      std::cerr << "MEAL::PhaseGradients<T>::calculate gradient" << std::endl
		<< "   " << (*grad)[igradient] << std::endl;
  }
  
}









#endif
