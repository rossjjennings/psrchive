/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/CalibratorType.h"
#include <iostream>

using namespace Pulsar;
using namespace std;

int main () try
{
  Calibrator::Type* test = Calibrator::Type::factory( "Britton" );
  cerr << "Calibrator::Type::identity = " << test->get_identity() << endl;
  return 0;
}
 catch (Error& error)
   {
     cerr << error << endl;
     return -1;
   }
