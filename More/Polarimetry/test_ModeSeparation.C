
#include "Pulsar/ModeSeparation.h"
#include "Pauli.h"

#include <iostream>
using namespace std;

int main () try
{
  Pulsar::ModeSeparation modes;

  Stokes<double> stokes (6,1,2,3);

  Jones<double> coherence = convert(stokes);
  Matrix<4,4,double> covariance = Mueller(coherence);

  modes.set_mean( stokes );
  modes.set_covariance ( covariance );

  modes.solve ();

  cerr << "modeA=" << modes.get_modeA()->get_stokes() << endl;
  cerr << "modeB=" << modes.get_modeB()->get_stokes() << endl;
  cerr << "modeC=" << modes.get_modeC()->get_stokes() << endl;

  cerr << "mean=" << coherency(modes.get_mean()->evaluate()) << endl;

  cerr << endl;

  cerr << "covariance=\n" << covariance << endl;
  cerr << "result=\n" << modes.get_covariance()->evaluate() << endl;

  return 0;
}
 catch (Error& error)
   {
     cerr << error << endl;
     return -1;
   }
