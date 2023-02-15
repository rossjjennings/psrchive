/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MethodFunction.h"

#include <iostream>
using namespace std;

class Test
{
  public:

  double get (int arg) { return arg * 1.5; }
};

int main ()
{
  auto thing = method_function (&Test::get, 1);

  Test test;

  double x = thing(&test);

  if ( x == 1.5 )
    return 0;
  else
    return -1;
}

