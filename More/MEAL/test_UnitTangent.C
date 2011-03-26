/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/UnitTangent.h"

#include <iostream>
#include <vector>
using namespace std;

int main (int argc, char** argv) try
{
  MEAL::UnitTangent unit;

  Vector<3,double> value = unit.evaluate();
  cerr << "default value=" << value << endl;

  unit.set_param(1, 1.0);

  std::vector< Vector<3,double> > gradient;
  value = unit.evaluate( &gradient );

  cerr << "after del1=1.0 value=" << value << endl;
  for (unsigned i=0; i<gradient.size(); i++)
    cerr << "grad[" << i << "]=" << gradient[i] << endl;

  return 0;
}
catch (Error & error)
{
  cerr << "test_UnitTangent: error" << error << endl;
  return -1;
}

