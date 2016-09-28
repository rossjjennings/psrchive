//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/units/Ranges.h

#ifndef __Ranges_H
#define __Ranges_H

#include <vector>
#include <iostream>

class Range
{
protected:
  bool gt, lt, eq;
  double x0,x1;

  void clear () { gt = lt = eq = false; }

public:
  Range () { clear(); x0 = x1 = 0; }

  friend std::ostream& operator<< (std::ostream&, const Range&);
  friend std::istream& operator>> (std::istream&, Range&);
  
  bool is_range () const { return !(eq || gt || lt); }
  std::pair<double,double> get_range () const;
  void set_range (double _0, double _1) { x0=_0; x1=_1; clear(); }

  bool is_value () const { return eq; }
  double get_value () const;
  void set_value (double x) { x0=x; clear(); eq=true; }

  bool within (double x) const;
};

class Ranges
{
protected:
  std::vector<Range> ranges;

public:
  friend std::ostream& operator<< (std::ostream&, const Ranges&);
  friend std::istream& operator>> (std::istream&, Ranges&);
  bool within (double x) const;
};

#endif // !__Ranges_H
