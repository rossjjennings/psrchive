/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Dirac.h"

using namespace std;

int main () 
{
  for (unsigned i=0; i<4; i++)
    for (unsigned j=0; j<4; j++)
      {
	Dirac::type M = Dirac::matrix (i, j);
	cout << i << " " << j << endl << M << endl << endl;
      }

  return 0;
}
