/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Ranges.h"
#include "Separator.h"
#include "tostring.h"

#include <list>
using namespace std;

ostream& operator<< (ostream& os, const Range& r)
{
  if (r.gt) os << ">" << r.x0;
  else if (r.lt) os << "<" << r.x0;
  else os << r.x0 << ":" << r.x1;
  return os;
}

istream& operator>> (istream& is, Range& r)
{
  r.lt = is.peek() == '<';
  r.gt = is.peek() == '>';

  if (r.lt || r.gt)
  {
    is.get();
    is >> r.x0;
    return is;
  }

  char separator = 0;
  is >> r.x0 >> separator >> r.x1;

  if (r.x0 > r.x1)
    std::swap (r.x0, r.x1);

  if ( separator != ':' )
    is.setstate (std::istream::failbit);

  return is;
}

std::pair<double,double> Range::get_range () const
{
  if (lt || gt)
    throw Error (InvalidState, "Range::get_range",
		 "range not described by finite bounds");

  return std::pair<double,double> (x0,x1);
}


bool Range::within (double x) const
{
  if (lt && x < x0)
    return true;

  if (gt && x > x0)
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
  string text;
  is >> text;
  r.parse (text);

  return is;
}

void Ranges::parse (const std::string& text)
{
  Separator separator;
  separator.set_preserve_numeric_ranges (false);
  separator.set_delimiters (",");

#if _DEBUG
  cerr << "Ranges::parse calling Separator::separate" << endl;
#endif

  list<string> tokens;
  separator.separate (text, tokens);

#if _DEBUG
  cerr << "Ranges::parse text=" << text << " ntokens=" << tokens.size() << endl;
#endif

  for (list<string>::iterator i=tokens.begin(); i != tokens.end(); i++)
    ranges.push_back( fromstring<Range>(*i) );
}

bool Ranges::within (double x) const
{
  for (unsigned i=0; i<ranges.size(); i++)
    if (ranges[i].within(x))
      return true;

  return false;
}

