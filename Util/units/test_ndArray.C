/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ndArray.h"

#include <iostream>
#include <cassert>

using namespace std;

int main ()
{
  ndArray<3,unsigned> data;

  int nrow=5;
  int ncol=4;
  int ndim=3;

  // resize an nrow by ncol by nchan 3-dimensional array
  data*nrow*ncol*ndim;

  cerr << "nrow=" << nrow << " ncol=" << ncol << " ndim=" << ndim << endl;
  cerr << "stride=" << data.stride() << endl;
  assert (data.stride() == 60);

  for (unsigned i=0; i<nrow; i++)
    for (unsigned j=0; j<ncol; j++)
      for (unsigned k=0; k<ndim; k++)
        data[i][j][k] = i*100 + j*10 + k;

  for (unsigned i=0; i<nrow; i++)
    for (unsigned j=0; j<ncol; j++)
      for (unsigned k=0; k<ndim; k++)
      {
        cerr << "data[" << i << "][" << j << "][" << k << "]=" << data[i][j][k] << endl;
        double expect = i*100 + j*10 + k;
        assert (data[i][j][k] == expect);
      }

  return 0;
}

