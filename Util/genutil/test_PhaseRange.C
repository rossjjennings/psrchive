/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "PhaseRange.h"
#include "tostring.h"

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
  }

  if (val.get_unit() != Phase::Milliseconds)
  {
    cerr << "test_PhaseRange ERROR"
      " unit=" << val.get_unit() << " != Milliseconds"
	 << endl;
  }

  cerr << "test_PhaseRange: All tests passed!" << endl;

  return 0;
}
 catch (Error& error)
   {
     cerr << "test_PhaseRange main: Error " << error << endl;
     return -1;
   }
