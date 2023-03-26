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
  enum Unit { Bins, Turns, Degrees, Radians,
	      Milliseconds, Seconds, Minutes, Hours };

  std::ostream& operator << (std::ostream&, Unit);
  std::istream& operator >> (std::istream&, Unit&);

  //! Base class of objects that have a Phase::Unit
  class HasUnit
  {
  protected:
    Unit unit;
    unsigned nbin;
    double period;
    bool insert_units;

    std::ostream& insertion (std::ostream&) const;
    std::istream& extraction (std::istream&);

  public:
    HasUnit (Unit u = Turns) { nbin=0; period=0; unit=u; insert_units = false; }
    
    unsigned get_bin (double value) const;
    double get_as (Unit, double value, bool round_down=false) const;
    double get_scale (Unit) const;

    void set_unit( Unit u ) { unit = u; }
    Unit get_unit () const { return unit; }

    void set_nbin( unsigned N ) { nbin = N; }

    //! Set the period in milliseconds
    void set_period( double P_ms ) { period = P_ms; }    
  };

  //! A value with a Phase::Unit
  class Value : public HasUnit
  {
    double value;

  public:
    Value (double v=0.0, Unit u=Turns) : HasUnit(u) { value = v; }

    void set_value( double v ) { value = v; }
    double get_value () const { return value; }

    unsigned get_bin () const { return HasUnit::get_bin(value); }
    double get_as (Unit u) const { return HasUnit::get_as(u, value); }

    Value as (Unit) const;

    friend std::ostream& operator<< (std::ostream&, const Value&);
    friend std::istream& operator>> (std::istream&, Value&);
  };

  //! A range of values with a Phase::Unit
  class Range : public HasUnit, public ::Range
  {
    ::Range& asRange () { return *this; }
    const ::Range& asRange () const { return *this; }

  public:

    Range () {}
    Range (const HasUnit& u, const ::Range& r) : HasUnit(u), ::Range(r) { }

    friend std::ostream& operator<< (std::ostream&, const Range&);
    friend std::istream& operator>> (std::istream&, Range&);
    
    std::pair<unsigned,unsigned> get_bins () const;
    std::pair<double,double> get_as (Unit) const;

    Range as (Unit) const;
  };

  //! A set of ranges of values with a common Phase::Unit
  class Ranges : public HasUnit, public ::Ranges
  {
    ::Ranges& asRanges () { return *this; }
    const ::Ranges& asRanges () const { return *this; }
 
  public:
    friend std::ostream& operator<< (std::ostream&, const Ranges&);
    friend std::istream& operator>> (std::istream&, Ranges&);

    Ranges as (Unit) const;
  };

  //! Policy for converting any object with a Phase::Unit to a string
  template<class Type>
  class ToStringPolicy
  {
  protected:

    mutable ToString tostr;
    mutable Phase::Unit unit;
    mutable bool unit_specified;

  public:

    ToStringPolicy () { unit_specified = false; }

    void set_modifiers (const std::string& modifiers) const
    {
#if _DEBUG
      std::cerr << "ToStringPolicy<Phase::HasUnit>::set_modifiers" << std::endl;
#endif

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
#if _DEBUG
      std::cerr << "ToStringPolicy<Phase::HasUnit>::operator" << std::endl;
#endif
      if (this->unit_specified)
	return tostr( t.as(this->unit) );
      else
	return tostr( t );
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

  //! Specialize the policy for converting a Range to a string
  template<>
  class ToStringPolicy<Phase::Range>
    : public Phase::ToStringPolicy<Phase::Range>
  {
  };

  //! Specialize the policy for converting a Range to a string
  template<>
  class ToStringPolicy<Phase::Ranges>
    : public Phase::ToStringPolicy<Phase::Ranges>
  {
  };
}

// Specialize the std C++ numeric_limits traits for Phase::* classes
namespace std {
  template<>
  class numeric_limits<Phase::Value> {
    public:
    static const int digits10 = numeric_limits<double>::digits10;
  };

  template<>
  class numeric_limits<Phase::Range> {
    public:
    static const int digits10 = numeric_limits<double>::digits10;
  };
  
  template<>
  class numeric_limits<Phase::Ranges> {
    public:
    static const int digits10 = numeric_limits<double>::digits10;
  };  
}

#endif // !__PhaseRanges_H
