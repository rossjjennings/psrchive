#include "MEAL/Polar.h"
#include "MEAL/Boost.h"
#include "MEAL/Rotation.h"

#include <iostream>

using namespace std;

int main ()
{
  MEAL::Function::verbose = true;

  double beta = 0.013;
  MEAL::Boost boost;
  boost.set_param (0, sinh(beta));

  if ( boost.evaluate()(0,0) != exp(beta) ||
       boost.evaluate()(1,1) != exp(-beta) )
    {
      cerr << "MEAL::Boost w/ beta=" << beta << " = " << boost.evaluate();
      cerr << "\nexp("<<beta<<")=" << exp(beta)
	   << "  exp("<<-beta<<")=" << exp(-beta) << endl;
      return -1;
    }


  double phi = 0.145;
  MEAL::Rotation rotation;
  rotation.set_param (0, phi);

  if ( fabs( rotation.evaluate()(0,0).real() - cos(phi) ) > 1e-10 ||
       fabs( rotation.evaluate()(0,0).imag() - sin(phi) ) > 1e-10 )
    {
      cerr << "MEAL::Rotation w/ phi=" << phi << " = " << rotation.evaluate();
      cerr << "\ncos("<<phi<<")=" << cos(phi)
	   << "  sin("<<phi<<")=" << sin(phi) << endl;
      return -1;
    }


  MEAL::Polar polar;

  if (polar.evaluate() != Jones<double>::identity()) {
    cerr << "MEAL::Polar default != identity" << endl;
    return -1;
  }

  double gain = 5.4;
  polar.set_param (0, gain);
  if (polar.evaluate() != Jones<double>::identity() * gain) {
    cerr << "MEAL::Polar w/ gain="<<gain<<" != identity*gain" << endl;
    return -1;
  }

  polar.set_param (1, sinh(beta));
  if ( norm(polar.evaluate() - boost.evaluate() * gain) > 1e-10 ) {
    cerr << "MEAL::Polar w/ gain="<<gain<<",beta="<<beta
	 <<" = " << polar.evaluate()
	 << " != " << boost.evaluate()*gain << endl;
    return -1;
  }

  polar.set_param (4, phi);
  double diff;
  diff = norm(polar.evaluate() - boost.evaluate()*rotation.evaluate()*gain);
  if (diff > 1e-10 ) {
    cerr << "MEAL::Polar w/ gain="<<gain<<",beta="<<beta
	 <<" = " << polar.evaluate()
	 << " != " << boost.evaluate()*rotation.evaluate()*gain << endl;
    return -1;
  }

  cerr << "MEAL::Polar passes all tests" << endl;

  return 0;
}
