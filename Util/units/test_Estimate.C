#include "Estimate.h"
#include "Stokes.h"

int main ()
{
  Estimate<float> e1 (5, 1);
  Estimate<float> e2 (7, 1);

  Estimate<float> e3;

  MeanEstimate<float> mean;

  mean += e1;
  mean += e2;

  e3 = mean.get_Estimate();

  cerr << "test_Estimate: mean of e1=" << e1 << " and e2=" << e2 <<
    "\n  = " << e3 << endl;

  if (e3.val != 6.0 || e3.var != 0.5) {
    cerr << "test_Estimate: MeanEstimate error " << e3 << endl;
    return -1;
  }

  Estimate<double> rad1 (M_PI/4.0, 0.01);
  Estimate<double> rad2 (2.0*M_PI-M_PI/4.0, 0.01);

  MeanRadian<double> radmean;

  radmean += rad1;
  radmean += rad2;

  Estimate<double> rad3 (radmean);

  cerr << "test_Estimate: mean of rad1=" << rad1 << " and rad2=" << rad2 <<
    "\n  = " << rad3 << endl;
    
  if (fabs(rad3.val) > 1e-10 || fabs(rad3.var - 0.005) > 1e-10) {
    cerr << "test_Estimate: MeanRadian error " << rad3 << endl;
    return -1;
  }

  return 0;
}

