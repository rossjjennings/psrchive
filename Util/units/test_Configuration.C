/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Configuration.h"
#include <iostream>

using namespace std;

int main () {

  Configuration config ("test.cfg");

  double f1 = config.get<double>("F1", 5.0);

  if (f1 != 3.0) {
    cerr << "F1 not parsed from test.cfg" << endl;
    return -1;
  }

  double f2 = config.get<double>("F2", 5.0);

  if (f2 != 5.0) {
    cerr << "Failure to set to default" << endl;
    return -1;
  }

  cerr << "All tests passed" << endl;

  return 0;
}
