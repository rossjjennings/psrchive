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

#include <list>
#include <math.h>

using namespace std;

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
    return nbin;
  default:
    throw Error (InvalidState, "Phase::HasUnit::get_scale",
		   "invalid Unit");
  }
}

unsigned Phase::HasUnit::get_bin (double value) const
{
  return get_as (Bins, value);
  if (unit == Bins)
    return value;
}

double Phase::HasUnit::get_as (Unit to, double value) const
{
  if (unit == to)
    return value;

  double result = value * get_scale(to) / get_scale (unit);

  if (to == Bins)
    result = round(result);

  return result;
}


std::ostream& Phase::HasUnit::insertion (std::ostream& os) const
{
  return os << "%" << unit;
}

std::istream& Phase::HasUnit::extraction (std::istream& is)
{
  if (is.peek() == '%')
  {
    is.get();
    is >> unit;
  }
  else
    unit = Turns;

  return is;
}

std::ostream& Phase::operator<< (std::ostream& os, const Value& v)
{
  os << v.value;
  return v.insertion (os);
}

std::istream& Phase::operator>> (std::istream& is, Value& v)
{
  is >> v.value;

  // cerr << "Phase::operator Value::value=" << v.value << endl;
  return v.extraction (is);
}

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
  Range range (*this);
  range.unit = u;
  range.x0 = HasUnit::get_as (u, x0);
  range.x1 = HasUnit::get_as (u, x1);
  return range;
}

Phase::Value Phase::Value::as (Unit u) const
{
  Value val (*this);
  val.unit = u;
  val.value = HasUnit::get_as (u, value);
  return val;
}


ostream& Phase::operator<< (ostream& os, const Phase::Ranges& r)
{
  for (unsigned i=0; i<r.ranges.size(); i++)
  {
    if (i>0)
      os << ",";
    os << r.ranges[i];
  }
  return os;
}

istream& Phase::operator>> (istream& is, Phase::Ranges& r)
{
  string text;
  is >> text;
  r.parse (text);

  return is;
}

void Phase::Ranges::parse (const std::string& text)
{
  Separator separator;
  separator.set_preserve_numeric_ranges (false);
  separator.set_delimiters (",");

#if _DEBUG
  cerr << "Phase::Ranges::parse calling Separator::separate" << endl;
#endif

  list<string> tokens;
  separator.separate (text, tokens);

#if _DEBUG
  cerr << "Phase::Ranges::parse text=" << text << " ntokens=" << tokens.size() << endl;
#endif

  for (list<string>::iterator i=tokens.begin(); i != tokens.end(); i++)
    ranges.push_back( fromstring<Phase::Range>(*i) );
}

bool Phase::Ranges::within (double x) const
{
  for (unsigned i=0; i<ranges.size(); i++)
    if (ranges[i].within(x))
      return true;

  return false;
}



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
  else if (unit == "bin")
    units = Phase::Bins;
  else
    is.setstate(std::istream::failbit);

  return is;
}
