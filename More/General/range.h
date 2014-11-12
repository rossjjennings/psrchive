//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2014 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/range.h

#ifndef __range_h
#define __range_h

class range
{
  bool gt, lt;
  double x0,x1;
public:
  range () { gt = lt = false; x0 = x1 = 0; }
  friend ostream& operator<< (ostream&, const range&);
  friend istream& operator>> (istream&, range&);
  bool within (double x) const;
};

#endif
