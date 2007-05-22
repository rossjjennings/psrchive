/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "psrephem.h"
//#include "T2Parameters.h"

using namespace std;

void Pulsar::Parameters::children (vector< Reference::To<Parameters> >& child)
{
  child.resize (0);

  child.push_back (new psrephem);
  // child.push_back (new Tempo2::Parameters);
}
