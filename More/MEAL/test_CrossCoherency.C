/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/CrossCoherency.h"
#include "MEAL/SingularCoherency.h"

#include "MEAL/SumRule.h"
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

  cerr << "mode A=" << A.evaluate() << endl;
  
  SingularCoherency B;
  B.set_vector( Vector<3,double> (-1,0,0) );
  cross.set_modeB (&B);

  cerr << "mode B=" << B.evaluate() << endl;

  correlation.set_value (0.0);
  cout << "cross coherency (incoherent) = " << cross.evaluate() << endl;

  correlation.set_value (1.0);
  cout << "cross coherency (coherent U) = " << cross.evaluate() << endl;

  correlation.set_value (complex<double>(0.0,1.0));
  cout << "cross coherency (coherent V) = " << cross.evaluate() << endl;

  SumRule<Coherency> tot;
  tot += &cross;
  tot += &A;

  cout << "cross nparam=" << cross.get_nparam() << endl;
  cout << "tot nparam=" << tot.get_nparam() << endl;

  /*
    While debugging the ModeSeparation class, it became clear that
    the CrossCoherency::get_stokes method is not working.

    The last two lines of the following block of code demonstrate the
    problem.

    Solution: the Coherency class should be turned into an abstract
    base class that is inherited by the various existing derived
    classes.

    Estimate<double> get_Stokes() should be converted into something
    that makes use of all parameter error estimates and partial
    derivatives with respect to those parameters.
  */

  Stokes<double> stokes (6,1,2,3);

  Vector<3,double> p = stokes.get_vector();
  double I = stokes.get_scalar();
  double P = norm(p);

  Stokes<double> a (I+P, (I+P)*p/P);
  a *= 0.5;
  A.set_stokes(a);

  cerr << "MODE A=" << A.get_stokes() << endl;

  Stokes<double> b (I-P, -(I-P)*p/P);
  b *= 0.5;
  B.set_stokes(b);

  cerr << "MODE B=" << B.get_stokes() << endl;


  correlation.set_value (0.0);
  cerr << "A+B=" << A.evaluate() + B.evaluate() << endl;
  cerr << "cross=" << cross.evaluate() << endl;
  cerr << "cross stokes=" << cross.get_stokes() << endl;

  return 0;
}

 catch (Error& error)
   {
     cerr << error << endl;
     return -1;
   }
