/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "FTransform.h"
#include "BoxMuller.h"

#include <stdio.h>
#include <math.h>

int main (int argc, char** argv)
{
  fprintf (stderr, "Test of real-to-complex and complex-to-real ...\n");

  int idat, ndat = 16 * 1024;  // 16kpt set of random, Gaussian noise
  float* data = new float [ndat];
  float* copy = new float [ndat];
  float* fft1 = new float [ndat+2];
  float* back = new float [ndat];

  long idum = -1;
  float dc_value = 3.0;

  fprintf (stderr, "Generating %d random numbers\n", ndat);

  BoxMuller gasdev;

  for (idat=0; idat<ndat; idat++)
    copy[idat] = data[idat] = dc_value + gasdev ();

  // ensure that DC and Nyquist are properly set by frc1d
  fft1[0] = fft1[ndat] = 0.0;   // should be set to other than zero
  fft1[1] = fft1[ndat+1] = 3.0; // should be set to zero

  fprintf (stderr, "Forward R->C FFT:%d\n", ndat);
  FTransform::frc1d (ndat, fft1, data);

  for (unsigned j=0; j < ndat; j++)
    if (copy[j] != data[j])  {
      fprintf(stderr,"idat=%d before=%f after=%f\n", j, data[j], copy[j]);
      fprintf(stderr,"Out-of-place transform does not preserve input!\n");
      return -1;
    }

  if (fft1[0] == 0.0) {
    fprintf (stderr, "Re[DC] = 0\n");
    return -1;
  }
  if (fft1[1] != 0.0) {
    fprintf (stderr, "Im[DC] != 0\n");
    return -1;
  }

  if (fft1[ndat] == 0.0) {
    fprintf (stderr, "Re[Nyquist] = 0\n");
    return -1;
  }
  if (fft1[ndat+1] != 0.0) {
    fprintf (stderr, "Im[Nyquist] != 0\n");
    return -1;
  }

  fprintf (stderr, "DC=%f,%f\n", fft1[0],fft1[1]);
  fprintf (stderr, "Ny=%f,%f\n", fft1[ndat],fft1[ndat+1]);

  fprintf (stderr, "Backward C->R FFT:%d\n", ndat);
  FTransform::bcr1d (ndat, back, fft1);

  for (idat=0; idat<ndat; idat++) {
    if (FTransform::get_norm() == FTransform::unnormalized)
      back[idat] /= ndat;
    float residual = (back[idat]-data[idat])/data[idat];
    if (fabs(residual) > 5e-4) {
      fprintf (stderr, "idat=%d data=%f back=%f diff=%g\n", 
               idat, back[idat], data[idat], residual);
      return -1;
    }
  }

  return 0;
}
