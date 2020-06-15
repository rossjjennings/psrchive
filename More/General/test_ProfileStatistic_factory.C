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

int main ()
{
  ProfileStatistic* stat = ProfileStatistic::factory ("iqr");  

  cerr << "stat=" << stat << endl;
  return 0;
}

