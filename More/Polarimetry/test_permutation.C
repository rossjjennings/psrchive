/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  This program numerically demonstrates that a rotation of 120 degrees about
  the (1,1,1) axis effects a cyclic permutation of the Stokes parameters
 */

#include "MEAL/Rotation.h"
#include "Pauli.h"

using namespace std;

int main (int argc, char** argv) try
{
  MEAL::Rotation permutation( {1, 1, 1}, -M_PI/3 );

  Jones<double> R = permutation.evaluate();

  cerr << "R=" << R << endl;

  const unsigned ndim=3;

  Stokes<double> bases[ndim];
  for (unsigned i=0; i<ndim; i++)
  {
    bases[i][0] = 2.0;   // total intensity
    bases[i][i+1] = 1.0; // 50% polarized
  }

  for (unsigned i=0; i<ndim; i++)
  {
    auto S = transform(bases[i], R);

    unsigned j = (i+1)%ndim;
    double diff = norm(S - bases[j]);

    if ( diff > 1e-15 )
    {
      cerr << "transformed basis[" << i+1 << "]=" << S << " != basis[" << j+1 << "]=" << bases[j] << " diff=" << diff << endl;
      return -1;
    }

    cerr << "transforms basis[" << i+1 << "]=" << bases[i] << " to basis[" << j+1 << "]=" << bases[j] << endl;
  }

  cerr << "all tests passed" << endl;
  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}

