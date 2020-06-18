/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStatistic.h"

#include <iostream>

using namespace std;
using namespace Pulsar;

int main () try
{
  ProfileStatistic* stat = ProfileStatistic::factory ("avg");  

  cerr << "stat=" << stat << endl;
  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}

