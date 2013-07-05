/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/Vectorize.h"
#include "MEAL/Union.h"

#include "MEAL/Boost.h"
#include "MEAL/Depolarizer.h"

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

  MEAL::Depolarizer depol;
  MEAL::Vectorize<MEAL::Real4> vdepol (&depol);

  for (unsigned i=0; i<vdepol.size(); i++)
  {
    vdepol.set_index(i);
    cout << "test_Vectorize: index=" << i 
	 << " val=" << vdepol.evaluate () << endl;
  }

  MEAL::Union space;
  space.push_back( &vboost );
  space.push_back( &vdepol );

  for (unsigned i=0; i<space.size(); i++)
  {
    space.set_index(i);
    cout << "test_Vectorize: index=" << i 
	 << " val=" << space.evaluate () << endl;
  }

  return 0;
}
catch (Error & error)
{
  cerr << "test_Vectorize: error" << error << endl;
  return -1;
}

