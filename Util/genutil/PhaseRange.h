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

    friend std::ostream& operator<< (std::ostream&, const Value&);
    friend std::istream& operator>> (std::istream&, Value&);
  };

  class Range : public HasUnit, public ::Range
  {
    ::Range& asRange () { return *this; }

  public:
    Range () { nbin=0; period=0; unit=Bins; }

    friend std::ostream& operator<< (std::ostream&, const Range&);
    friend std::istream& operator>> (std::istream&, Range&);
    
    void set_nbin( unsigned N ) { nbin = N; }
    void set_period( double P ) { period = P; }
    
    std::pair<unsigned,unsigned> get_bins () const;
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
}

#endif // !__PhaseRanges_H
