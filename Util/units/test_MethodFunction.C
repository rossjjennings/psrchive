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
  double factor;

  public:
  Test () { factor = 1.5; }

  double get (int arg) { return arg * factor; }
  void set (int index, double value) { factor = value; }
};

int main ()
{
  auto get_function = method_function (&Test::get, 1);

  Test test;

  double x = get_function(&test);

  if ( x == 1.5 )
    return 0;
  else
    return -1;

  auto set_function = method_function (&Test::set, 2);

  set_function (&test, 3.5);
  x = get_function(&test);

  if ( x == 3.5 )
    return 0;
  else
    return -1;
}

