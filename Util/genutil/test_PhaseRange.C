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

/***************************************************************************
 *
 *   Phase::Value
 *
 ***************************************************************************/

int test_PhaseValue ()
{
  Phase::Value val;

  cerr << "test_PhaseRange Value in Milliseconds" << endl;

  val = fromstring<Phase::Value> ("0.03%ms");

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

  cerr << "test_PhaseRange Value::get_nbin" << endl;

  val.set_period (1.0);
  val.set_nbin (1000);
  if (val.get_bin() != 30)
  {
    cerr << "test_PhaseRange ERROR"
      " bin=" << val.get_bin() << " != 30"
	 << endl;
    return -1;
  }
return 0;
}

/***************************************************************************
 *
 *   Phase::Range
 *
 ***************************************************************************/

int test_PhaseRange ()
{
  cerr << "test_PhaseRange Range in Degrees" << endl;

  Phase::Range range;

  range = fromstring<Phase::Range> ("10:180%deg");

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
      " unit=" << range.get_unit() << " != Degrees"
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

  return 0;
}

int test_PhaseRange_as_Value ()
{
  cerr << "test_PhaseRange Range as value" << endl;

  Phase::Range range = fromstring<Phase::Range> ("1.23%rad");

  double val = range.get_value();
  if (val != 1.23)
  {
    cerr << "test_PhaseRange ERROR"
      " pair=" << val << " != 1.23"
	 << endl;
    return -1;
  }

  if (range.get_unit() != Phase::Radians)
  {
    cerr << "test_PhaseRange ERROR"
      " unit=" << range.get_unit() << " != Radians"
	 << endl;
    return -1;
  }

  return 0;
}

/***************************************************************************
 *
 *   Phase::Ranges
 *
 ***************************************************************************/

int test_PhaseRanges ()
{
  cerr << "test_PhaseRange Range in Degrees" << endl;

  Phase::Ranges ranges;

  ranges = fromstring<Phase::Ranges> ("10:180,270,330-360%deg");

  if (ranges.get_unit() != Phase::Degrees)
  {
    cerr << "test_PhaseRange ERROR"
      " unit=" << ranges.get_unit() << " != Degrees"
	 << endl;
    return -1;
  }

  ranges.set_nbin( 1024 );

  Phase::Ranges bins = ranges.as( Phase::Bins );

  ranges = fromstring<Phase::Ranges> ("10:180");

  return 0;
}


int main () try
{
  if (test_PhaseValue() < 0)
    return -1;

  if (test_PhaseRange() < 0)
    return -1;

  if (test_PhaseRange_as_Value() < 0)
    return -1;

  if (test_PhaseRanges() < 0)
    return -1;

  cerr << "test_PhaseRange: All tests passed!" << endl;

  return 0;
}
 catch (Error& error)
   {
     cerr << "test_PhaseRange main: Error " << error << endl;
     return -1;
   }
