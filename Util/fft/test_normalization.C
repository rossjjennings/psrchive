#include <stdio.h>
#include <math.h>

#include "fftm.h"
#include "spectra.h"

int main (int argc, char** argv)
{
  fprintf (stderr, "Test of normalization factors...\n");

  int idat, ndat = 16 * 1024;  // 16kpt set of random, Gaussian noise
  float* data = new float [ndat];
  float* fft1 = new float [ndat];
  float* fft2 = new float [ndat];

  int ntests = 4;
  int nchan[4] = {8, 16, 32, 64};
  long idum = -1;

  fprintf (stderr, "Generating %d random numbers\n", ndat);
  gasdev (&idum);
  for (idat=0; idat<ndat; idat++)
    data[idat] = gasdev (&idum);

  int ntrans = ndat/2;
  fprintf (stderr, "Forward C->C FFT:%d\n", ntrans);
  fft::fcc1d (ntrans, fft1, data);

  fprintf (stderr, "Backward C->C FFT:%d\n", ntrans);
  fft::bcc1d (ntrans, fft2, fft1);

  float factor1 = 0.0;
  for (idat=0; idat<ndat; idat++)
    factor1 += fft2[idat]/data[idat];
  factor1 /= ndat;

  float powerin = 0.0;
  for (idat=0; idat<ndat; idat++)
    powerin += data[idat]*data[idat];

  float powerout = 0.0;
  for (idat=0; idat<ndat; idat++)
    powerout += fft2[idat]*fft2[idat];

  long factor = long (rint (powerout/powerin));
  long expect1 = ndat*ntrans/2;
  long expect2 = ndat/(ntrans*2);

  fprintf (stderr, "For %d pts, POWER out/in=%ld  MEAN out/in=%d\n",
	   ntrans, factor, int(rint(factor1)));
  fprintf (stderr, "%d X %d = %ld\n", ndat/2, ntrans, expect1);
  fprintf (stderr, "%d / %d = %ld\n", ndat/2, ntrans, expect2);

  // Now try taking a sub-band...  although the power should be
  // divided evenly into each channel, perform the test for each

  for (int j=0; j<ntests; j++) {
    fprintf (stderr, "\nForward:%dpt followed by %d sub-channels: \n",
	     ndat/2, nchan[j]);
    int bdat = ndat / nchan[j];
    ntrans = bdat / 2;
    fprintf (stderr, "Backward C->C FFT:%d\n", ntrans);

    powerout = 0.0;
    for (int ichan=0; ichan<nchan[j]; ichan++) {
      fft::bcc1d (ntrans, fft2, fft1+ichan*bdat);

      // find the average multiplicative factor
      float chan_power = 0.0;
      for (idat=0; idat<bdat; idat++)
	chan_power += fft2[idat]*fft2[idat];

      //fprintf (stderr, "Channel %d, power=%f\n", ichan, chan_power);
      powerout += chan_power;
    }
    factor = long (rint (powerout/powerin));
    expect1 = ndat*ntrans/2;
    expect2 = ndat/(ntrans*2);

    fprintf (stderr, "For %d pts, POWER out/in=%ld\n",
	     ntrans, factor);
    fprintf (stderr, "%d X %d = %ld\n", ndat/2, ntrans, expect1);
    fprintf (stderr, "%d / %d = %ld\n", ndat/2, ntrans, expect2);
  }
  return 0;
}
