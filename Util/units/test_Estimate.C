#include "Estimate.h"

int main ()
{
  Estimate<float> e1 (-13e2, 1);
  Estimate<float> e2 (4.5, 3e-5);

  Estimate<float> e3;

  MeanEstimate<float> mean;

  e3 = e1 * e2;
  e3 = e1 / e2;

  mean += e1;
  mean += e2;

  e3 = mean.get_Estimate();

  return 0;
}

