
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

  cerr << "modeA=" << coherency(modes.get_modeA()->evaluate()) << endl;
  cerr << "modeB=" << coherency(modes.get_modeB()->evaluate()) << endl;
  cerr << "modeC=" << coherency(modes.get_modeC()->evaluate()) << endl;
  cerr << "correlation=" << modes.get_correlation()->evaluate() << endl;
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
