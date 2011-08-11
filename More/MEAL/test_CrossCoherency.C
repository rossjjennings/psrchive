/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/CrossCoherency.h"
#include "MEAL/SingularCoherency.h"
#include "MEAL/Value.h"

using namespace std;
using namespace MEAL;

int main () try
{
  //Function::very_verbose = true;
  //Function::verbose = true;

  CrossCoherency cross;

  cerr << "constructed" << endl;

  Value<Complex> correlation;
  cross.set_correlation (&correlation);

  cerr << "correlation added" << endl;

  SingularCoherency A;
  A.set_vector( Vector<3,double> (1,0,0) );
  cross.set_modeA (&A);

  cerr << "mode A set" << endl;
  
  SingularCoherency B;
  B.set_vector( Vector<3,double> (-1,0,0) );
  cross.set_modeB (&B);

  cerr << "mode B set" << endl;

  correlation.set_value (0.0);
  cout << "cross coherency (incoherent) = " << cross.evaluate() << endl;

  correlation.set_value (1.0);
  cout << "cross coherency (coherent U) = " << cross.evaluate() << endl;

  correlation.set_value (complex<double>(0.0,1.0));
  cout << "cross coherency (coherent V) = " << cross.evaluate() << endl;

  return 0;
}

 catch (Error& error)
   {
     cerr << error << endl;
     return -1;
   }
