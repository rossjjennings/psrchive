/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "PhaseRange.h"
#include "Separator.h"
#include "tostring.h"
#include "stringtok.h"

#include <list>
#include <math.h>

using namespace std;

/***************************************************************************
 *
 *   Phase::Unit
 *
 ***************************************************************************/

std::ostream& Phase::operator << (std::ostream& os, Unit units)
{
  switch (units) {
  case Phase::Turns:
    return os << "turn";
  case Phase::Degrees:
    return os << "deg";
  case Phase::Radians:
    return os << "rad";
  case Phase::Milliseconds:
    return os << "ms";
  case Phase::Seconds:
    return os << "s";
  case Phase::Minutes:
    return os << "min";
  case Phase::Hours:
    return os << "hr";
  case Phase::Bins:
    return os << "bin";
  default:
    return os << "unknown";
  }
}

std::istream& Phase::operator >> (std::istream& is, Phase::Unit& units)
{
  std::streampos pos = is.tellg();
  std::string unit;
  is >> unit;

  if (unit == "turn")
    units = Phase::Turns;
  else if (unit == "deg")
    units = Phase::Degrees;
  else if (unit == "rad")
    units = Phase::Radians;
  else if (unit == "ms")
    units = Phase::Milliseconds;
  else if (unit == "s")
    units = Phase::Seconds;
  else if (unit == "min")
    units = Phase::Minutes;
  else if (unit == "hr")
    units = Phase::Hours;
  else if (unit == "bin")
    units = Phase::Bins;
  else {
    is.setstate(std::istream::failbit);
    is.seekg (pos);
  }

  return is;
}

/***************************************************************************
 *
 *   Phase::HasUnit
 *
 ***************************************************************************/

double Phase::HasUnit::get_scale (Unit u) const
{
  switch (u)
  {
  case Phase::Turns:
    return 1.0;
  case Phase::Degrees:
    return 360.0;
  case Phase::Radians:
    return 2 * M_PI;
  case Phase::Milliseconds:
    if (!period)
      throw Error (InvalidState, "Phase::HasUnit::get_scale",
		   "Unit == Milliseconds and period not set");
    return period;
  case Phase::Bins:
    if (!nbin)
      throw Error (InvalidState, "Phase::HasUnit::get_scale",
		   "Unit == Bins and nbin not set");
    return nbin-1;
  default:
    throw Error (InvalidState, "Phase::HasUnit::get_scale",
		   "invalid Unit");
  }
}

unsigned Phase::HasUnit::get_bin (double value) const
{
  if (unit == Bins)
    return value;

  return get_as (Bins, value);
}

double Phase::HasUnit::get_as (Unit to, double value, bool round_down) const
{
  if (unit == to)
    return value;

  double result = value * get_scale(to) / get_scale (unit);

  if (to != Bins)
    return result;

  if (round_down)
    return floor (result);
  else
    return round (result);
}

std::ostream& Phase::HasUnit::insertion (std::ostream& os) const
{
  if (insert_units)
    os << "%" << unit;

  return os;
}

std::istream& Phase::HasUnit::extraction (std::istream& is)
{
  unit = Turns;
  insert_units = false;

  if (!is.eof() && is.peek() == '%')
  {
    is.get();

    if (!is.eof() && is.peek() == '.')
    {
      is.get();
      insert_units = false;
    }

    is >> unit;
  }

  return is;
}

/***************************************************************************
 *
 *   Phase::Value
 *
 ***************************************************************************/

std::ostream& Phase::operator<< (std::ostream& os, const Value& v)
{
  os << v.value;
  return v.insertion (os);
}

std::istream& Phase::operator>> (std::istream& is, Value& v)
{
  is >> v.value;

  return v.extraction (is);
}

Phase::Value Phase::Value::as (Unit u) const
{
  if (unit == u)
    return *this;

  Value val (*this);
  val.unit = u;
  val.value = HasUnit::get_as (u, value);
  return val;
}

/***************************************************************************
 *
 *   Phase::Range
 *
 ***************************************************************************/

ostream& Phase::operator<< (ostream& os, const Phase::Range& r)
{
  os << r.asRange();
  return r.insertion (os);
}

istream& Phase::operator>> (istream& is, Phase::Range& r)
{
  is >> r.asRange();
  return r.extraction (is);
}

std::pair<unsigned,unsigned> Phase::Range::get_bins () const
{
  std::pair<double,double> range = get_range();
  return std::pair<unsigned,unsigned>( get_bin(range.first),
				       get_bin(range.second) );
}

Phase::Range Phase::Range::as (Unit u) const
{
  if (unit == u)
    return *this;

  Range range (*this);
  range.unit = u;
  range.x0 = HasUnit::get_as (u, x0, true);
  range.x1 = HasUnit::get_as (u, x1);
  return range;
}


/***************************************************************************
 *
 *   Phase::Ranges
 *
 ***************************************************************************/

ostream& Phase::operator<< (ostream& os, const Phase::Ranges& r)
{
  os << r.asRanges();
  return r.insertion (os);
}

istream& Phase::operator>> (istream& is, Phase::Ranges& r)
{
  is >> r.asRanges();
  return r.extraction (is);
}

Phase::Ranges Phase::Ranges::as (Unit u) const
{
  if (unit == u)
    return *this;

  Ranges result (*this);

  for (unsigned i=0; i<ranges.size(); i++)
  {
    Range r (*this, ranges[i]);
    result.ranges[i] = r.as(u);
  }

  result.unit = u;

  return result;
}
