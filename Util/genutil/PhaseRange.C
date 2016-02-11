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
using namespace std;


std::ostream& Phase::HasUnit::insertion (std::ostream& os) const
{
  return os << "_" << unit;
}

std::istream& Phase::HasUnit::extraction (std::istream& is)
{
  if (is.peek() == '_')
  {
    is.get();
    is >> unit;
  }
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

  cerr << "Phase::operator Value::value=" << v.value << endl;
  return v.extraction (is);
}

ostream& Phase::operator<< (ostream& os, const Phase::Range& r)
{
  os << static_cast<const ::Range&> (r);
  return r.insertion (os);
}

istream& Phase::operator>> (istream& is, Phase::Range& r)
{
  is >> r.asRange();
  return r.extraction (is);
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
