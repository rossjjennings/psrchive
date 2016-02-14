/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#define _DEBUG 1

#include "Ranges.h"
#include "Separator.h"
#include "tostring.h"

#include <list>
using namespace std;

ostream& operator<< (ostream& os, const Range& r)
{
  if (r.gt) os << ">" << r.x0;
  else if (r.lt) os << "<" << r.x0;
  else if (r.eq) os << r.x0;
  else os << r.x0 << ":" << r.x1;
  return os;
}

istream& operator>> (istream& is, Range& r)
{
  r.lt = is.peek() == '<';
  r.gt = is.peek() == '>';
  r.eq = false;

  if (r.lt || r.gt)
  {
    is.get();
    is >> r.x0;
    return is;
  }

  is >> r.x0;
  
  // cerr << "operator>>Range got x0=" << r.x0 << endl;

  r.eq = is.eof() || !(is.peek() == ':' || is.peek() == '-');
  if (r.eq)
  {
    // cerr << "operator>>Range single value" << endl;
    return is;
  }

  // cerr << "operator>>Range getting x1" << endl;

  is.get();
  is >> r.x1;

  // cerr << "operator>>Range got x1=" << r.x1 << endl;

  return is;
}

std::pair<double,double> Range::get_range () const
{
  if (lt || gt || eq)
    throw Error (InvalidState, "Range::get_range",
		 "range not described by finite bounds");

  return std::pair<double,double> (x0,x1);
}

double Range::get_value () const
{
  if (!eq)
    throw Error (InvalidState, "Range::get_value",
		 "range is not a single value");

  return x0;
}


bool Range::within (double x) const
{
  if (lt && x < x0)
    return true;

  if (gt && x > x0)
    return true;

  if (eq && x == x0)
    return true;

  return (x>x0 && x<x1);
}

ostream& operator<< (ostream& os, const Ranges& r)
{
  for (unsigned i=0; i<r.ranges.size(); i++)
  {
    if (i>0)
      os << ",";
    os << r.ranges[i];
  }
  return os;
}

istream& operator>> (istream& is, Ranges& r)
{
  do {
    Range range;
    is >> range;
    r.ranges.push_back(range);
  } 
  while (!is.eof() && is.peek() == ',' && is.get());

  return is;
}

bool Ranges::within (double x) const
{
  for (unsigned i=0; i<ranges.size(); i++)
    if (ranges[i].within(x))
      return true;

  return false;
}

