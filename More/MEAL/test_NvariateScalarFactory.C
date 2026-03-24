/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/NvariateScalarFactory.h"

#include <iostream>
using namespace std;

int main (int argc, char** argv) try
{
  MEAL::NvariateScalarFactory factory;

  if (argc < 2)
  {
    cerr << "test_NvariateScalar_load: please provide name of file to load" << endl;
    factory ("help");
    return -1;
  }

  MEAL::Nvariate<MEAL::Scalar>* function = factory( argv[1] );

  string text;
  function->print( text );

  cerr << "test_NvariateScalar_load: loaded\n" << text << endl;

  return 0;
}
catch (Error & error)
{
  cerr << "test_NvariateScalar_load: error" << error << endl;
  return -1;
}

