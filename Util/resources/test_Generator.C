/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Generator.h"

int main() try
{
  Pulsar::Generator* generator = Pulsar::Generator::get_default ();
  (void) generator;
  return 0;
}
catch (...)
{
  return -1;
}

