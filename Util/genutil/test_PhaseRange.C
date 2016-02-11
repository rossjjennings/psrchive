/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "PhaseRange.h"
#include "tostring.h"
#include "pairutil.h"

#include <iostream>
using namespace std;

int main () try
{
  Phase::Value val;

  val = fromstring<Phase::Value> ("0.03_ms");

  if (val.get_value() != 0.03)
  {
    cerr << "test_PhaseRange ERROR"
      " value=" << val.get_value() << " != 0.03"
	 << endl;
    return -1;
  }

  if (val.get_unit() != Phase::Milliseconds)
  {
    cerr << "test_PhaseRange ERROR"
      " unit=" << val.get_unit() << " != Milliseconds"
	 << endl;
    return -1;
  }

  val.set_period (1.0);
  val.set_nbin (1000);
  if (val.get_bin() != 30)
  {
    cerr << "test_PhaseRange ERROR"
      " bin=" << val.get_bin() << " != 30"
	 << endl;
    return -1;
  }

  Phase::Range range;

  range = fromstring<Phase::Range> ("10:180_deg");

  std::pair<double,double> vals = range.get_range();
  if (vals.first != 10.0 || vals.second != 180.0)
  {
    cerr << "test_PhaseRange ERROR"
      " pair=" << vals << " != 10:180"
	 << endl;
    return -1;
  }

  if (range.get_unit() != Phase::Degrees)
  {
    cerr << "test_PhaseRange ERROR"
      " unit=" << val.get_unit() << " != Degrees"
	 << endl;
    return -1;
  }

  range.set_nbin (720);
  std::pair<unsigned,unsigned> bins = range.get_bins();
  if (bins.first != 20 || bins.second != 360)
  {
    cerr << "test_PhaseRange ERROR"
      " pair=" << bins << " != 20:360"
	 << endl;
    return -1;
  }

  cerr << "test_PhaseRange: All tests passed!" << endl;

  return 0;
}
 catch (Error& error)
   {
     cerr << "test_PhaseRange main: Error " << error << endl;
     return -1;
   }
