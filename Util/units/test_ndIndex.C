/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ndIndex.h"

#include <iostream>
using namespace std;

int main ()
{
  ndIndex<3> idx;

  int nrow=5;
  int ncol=4;
  int ndim=3;

  idx^nrow^ncol^ndim;

  cerr << "nrow=" << nrow << " ncol=" << ncol << " ndim=" << ndim << endl;
  cerr << "stride=" << idx.stride() << endl;

  cerr << "idx[0][0][0]=" << idx[0][0][0] << endl << endl;

  cerr << "idx[1][0][0]=" << idx[1][0][0] << endl;
  cerr << "idx[2][0][0]=" << idx[2][0][0] << endl << endl;

  cerr << "idx[0][1][0]=" << idx[0][1][0] << endl;
  cerr << "idx[0][2][0]=" << idx[0][2][0] << endl << endl;

  cerr << "idx[0][0][1]=" << idx[0][0][1] << endl;
  cerr << "idx[0][0][2]=" << idx[0][0][2] << endl << endl;

  return 0;
}
