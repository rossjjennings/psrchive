/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/JonesMueller.h"
#include "MEAL/Coherency.h"

using namespace std;
using namespace MEAL;

int main () try
{
  JonesMueller M;
  Coherency rho;

  M.set_transformation( &rho );

  Stokes<double> S (6,1,2,3);
  rho.set_stokes (S);

  double norm = S[0]*S[0] + S.sqr_vect();

  cerr << "S=" << S << " norm=" << norm << endl;
  cerr << "M=\n" << M.evaluate() << endl;

  return 0;
}

 catch (Error& error)
   {
     cerr << error << endl;
     return -1;
   }
