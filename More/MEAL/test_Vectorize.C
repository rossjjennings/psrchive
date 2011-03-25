/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Vectorize.h"
#include "MEAL/Boost.h"

#include <iostream>
using namespace std;

int main (int argc, char** argv) try
{
  MEAL::Boost boost;

  MEAL::Vectorize<MEAL::Complex2> vboost (&boost);

  for (unsigned i=0; i<vboost.size(); i++)
  {
    vboost.set_index(i);
    cout << "test_Vectorize: index=" << i 
	 << " val=" << vboost.evaluate () << endl;
  }

  return 0;
}
catch (Error & error)
{
  cerr << "test_Vectorize: error" << error << endl;
  return -1;
}

