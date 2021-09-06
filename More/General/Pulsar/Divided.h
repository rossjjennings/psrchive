//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Divided.h

#ifndef __Pulsar_Divided_h
#define __Pulsar_Divided_h

#include "Pulsar/Integrate.h"

namespace Pulsar {

  template<class C>
  class Integrate<C>::Divided : public Integrate<C>::RangePolicy {

  public:

    //! Default constructor
    Divided ();

    //! Set the number of ranges
    void set_ndivide (unsigned ndivide);

    //! Get the number of ranges
    unsigned get_ndivide () const;

    //! Set the number of container elements to integrate
    void set_nintegrate (unsigned nintegrate);

    //! Get the number of container elements to integrate
    unsigned get_nintegrate () const;

    //! Compute a roughly even division of ndivide into ranges
    void divide (unsigned ndivide, 
		 unsigned& nrange, unsigned& nintegrate) const;

  protected:

    //! Derived types must return the size of the container
    virtual unsigned get_size (const C*) = 0;

    //! The number of output elements
    unsigned ndivide;

    //! The number of elements to integrate
    unsigned nintegrate;

  };

}

//! Default constructor
template<class C>
Pulsar::Integrate<C>::Divided::Divided ()
{
  ndivide = 0;
  nintegrate = 0;
}

//! Set the number of output elements
template<class C>
void Pulsar::Integrate<C>::Divided::set_ndivide (unsigned _ndivide)
{
  ndivide = _ndivide;
  nintegrate = 0;
}

//! Get the number of output elements
template<class C>
unsigned Pulsar::Integrate<C>::Divided::get_ndivide () const
{
  return ndivide;
}

//! Set the number of channels to integrate
template<class C>
void Pulsar::Integrate<C>::Divided::set_nintegrate (unsigned _nintegrate)
{
  nintegrate = _nintegrate;
  ndivide = 0;
}

//! Get the number of channels to integrate
template<class C>
unsigned Pulsar::Integrate<C>::Divided::get_nintegrate () const
{
  return nintegrate;
}

template<class C>
void Pulsar::Integrate<C>::Divided::divide (unsigned input_ndivide,
					    unsigned& output_ndivide,
					    unsigned& output_nintegrate) const
{
  output_ndivide = 1;
  output_nintegrate = input_ndivide;

  if (ndivide)
  {

#if _DEBUG
      std::cerr << "Pulsar::Integrate::Divided::divide ndivide=" << ndivide
		<< std::endl;
#endif

    // the number of output elements was specified
    output_ndivide = ndivide;
    // calculate the output_nintegrate
    output_nintegrate = input_ndivide / output_ndivide;
    if (input_ndivide % output_ndivide)
      output_nintegrate ++;

  }
  else if (nintegrate)
  {

#if _DEBUG
      std::cerr << "Pulsar::Integrate::Divided::divide nintegrate="
		<< nintegrate << std::endl; 
#endif

    // the number of elements to integrate was specified
    output_nintegrate = nintegrate;
    // calculate the number of output elements
    output_ndivide = input_ndivide / output_nintegrate;
    if (input_ndivide % output_nintegrate)
      output_ndivide ++;

  }

#if _DEBUG
    std::cerr << "Pulsar::Integrate::Divided::divide into " << output_ndivide 
	      << " ranges with " << output_nintegrate << " elements per range"
	      << std::endl;
#endif
}


#endif
