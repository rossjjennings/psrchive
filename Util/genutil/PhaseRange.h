//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/PhaseRanges.h

#ifndef __PhaseRanges_H
#define __PhaseRanges_H

#include "Ranges.h"
#include "TextInterfaceAttribute.h"

namespace Phase
{
  //! The units of Phase
  enum Unit { Turns, Degrees, Radians, Milliseconds, Bins };

  std::ostream& operator << (std::ostream&, Unit);
  std::istream& operator >> (std::istream&, Unit&);

  class HasUnit
  {
  protected:
    Unit unit;
    unsigned nbin;
    double period;

    std::ostream& insertion (std::ostream&) const;
    std::istream& extraction (std::istream&);

  public:
    HasUnit () { nbin=0; period=0; unit=Turns; }
    
    unsigned get_bin (double value) const;
    double get_as (Unit, double value) const;
    double get_scale (Unit) const;

    void set_unit( Unit u ) { unit = u; }
    Unit get_unit () const { return unit; }

    void set_nbin( unsigned N ) { nbin = N; }
    void set_period( double P ) { period = P; }    
  };

  class Value : public HasUnit
  {
    double value;

  public:
    Value (double v=0.0) { value = v; }

    void set_value( double v ) { value = v; }
    double get_value () const { return value; }

    unsigned get_bin () const { return HasUnit::get_bin(value); }
    double get_as (Unit u) const { return HasUnit::get_as(u, value); }

    Value as (Unit) const;

    friend std::ostream& operator<< (std::ostream&, const Value&);
    friend std::istream& operator>> (std::istream&, Value&);
  };

  class Range : public HasUnit, public ::Range
  {
    ::Range& asRange () { return *this; }
    const ::Range& asRange () const { return *this; }

  public:

    friend std::ostream& operator<< (std::ostream&, const Range&);
    friend std::istream& operator>> (std::istream&, Range&);
    
    void set_nbin( unsigned N ) { nbin = N; }
    void set_period( double P ) { period = P; }
    
    std::pair<unsigned,unsigned> get_bins () const;
    std::pair<double,double> get_as (Unit) const;

    Range as (Unit) const;
  };

  class Ranges
  {
    std::vector<Range> ranges;
    
  public:
    friend std::ostream& operator<< (std::ostream&, const Ranges&);
    friend std::istream& operator>> (std::istream&, Ranges&);
    bool within (double x) const;
    void parse (const std::string& text);
  };

  //! Policy for converting a Range to a string
  template<class Type>
  class ToStringPolicy
  {
  protected:

    mutable ToString tostring;
    mutable Phase::Unit unit;
    mutable bool unit_specified;

  public:

    ToStringPolicy () { unit_specified = false; }

    void set_modifiers (const std::string& modifiers) const
    {
      std::cerr << "ToStringPolicy<Phase::HasUnit>::set_modifiers" << std::endl;

      unit = fromstring<Phase::Unit> (modifiers);
      unit_specified = true;
    }

    void reset_modifiers () const
    { 
      unit_specified = false;
      // tostring.reset_modifiers ();
    }

    std::string operator () (const Type& t) const
    {
      std::cerr << "ToStringPolicy<Phase::HasUnit>::operator" << std::endl;
      if (this->unit_specified)
	return tostring( t.as(this->unit) );
      else
	return tostring( t );
    }
  };
}

namespace TextInterface
{
  //! Specialize the policy for converting a Value to a string
  template<>
  class ToStringPolicy<Phase::Value> 
    : public Phase::ToStringPolicy<Phase::Value>
  {
  };

  //! Specialize th policy for converting a Range to a string
  template<>
  class ToStringPolicy<Phase::Range>
    : public Phase::ToStringPolicy<Phase::Range>
  {
  };
}

#endif // !__PhaseRanges_H
