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
  bool gt, lt;
  double x0,x1;
public:
  Range () { gt = lt = false; x0 = x1 = 0; }

  friend std::ostream& operator<< (std::ostream&, const Range&);
  friend std::istream& operator>> (std::istream&, Range&);
  
  std::pair<double,double> get_range () const;
  void set_range (double _0, double _1) { x0=_0; x1=_1; }

  bool within (double x) const;
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

#endif // !__Ranges_H
