/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <iostream>
using namespace std;

#include <stdio.h>
#include <math.h>

#include "fftm.h"
#include "spectra.h"

void runtest (int ndat);

int main (int argc, char** argv)
{
  fprintf (stderr, "Test of normalization factors...\n");

  // from 16kpt
  int ndat = 16 * 1024;

  for (unsigned i=0; i < 3; i++) {
    runtest (ndat);
    ndat *= 4;
  }

  cerr << "All normalization tests passed" << endl;
}

static long idum = -1;

double power (unsigned ndat, float* data);

void runtest2 (int ntrans, double powerin, float* fft1, float* data = 0);

void runtest (int ndat)
{
  float* data = new float [ndat];
  float* fft1 = new float [ndat + 2];

  fprintf (stderr, "Generating %d random numbers\n", ndat);
  gasdev (&idum);

  for (int idat=0; idat<ndat; idat++)
    data[idat] = gasdev (&idum);

  int ntrans = ndat/2;

  fprintf (stderr, "Forward R->C FFT:%d\n", ntrans);
  fft::frc1d (ndat, fft1, data);

  runtest2 (ntrans, power(ndat+1,data), fft1);

  fprintf (stderr, "Forward C->C FFT:%d\n", ntrans);
  fft::fcc1d (ntrans, fft1, data);

  runtest2 (ntrans, power(ndat,data), fft1, data);
}

double power (unsigned ndat, float* data)
{
  double p = 0;
  double mean = 0;
  for (unsigned idat=0; idat<ndat; idat++) {
    p += data[idat]*data[idat];
    mean += data[idat];
  }
  mean /= ndat;
  return p - mean*mean;
}

double diff (unsigned ndat, float* in, float* out)
{
  double c = 0.0;
  double scale = (fft::get_normalization() == fft::nfft) ? ndat/2 : 1;

  cerr << "scale=" << scale << endl;

  for (unsigned idat=0; idat<ndat; idat++) {
    double d = scale*in[idat] - out[idat];
    c += d * d;
  }

  return c / ndat / scale;
}

void test (const char* name, double got, double expect)
{
  fprintf (stderr, "POWER fft%s/in=%lf - expect %lf\n", name, got, expect);
  if ( fabs(got-expect)/expect > 1e-4 ) {
    fprintf (stderr, "normalization test %s failed\n", name);
    exit (-1);
  }
}

void runtest2 (int ntrans, double powerin, float* fft1, float* data)
{
  float* fft2 = new float [ntrans*2];

  fprintf (stderr, "Backward C->C FFT:%d\n", ntrans);
  fft::bcc1d (ntrans, fft2, fft1);

  if (data) {
    double d = diff (ntrans*2, data, fft2);
    fprintf (stderr, "For %d pts: <(out-in)^2> = %lf\n", ntrans, d);
    if (d > 1e-6) {
      fprintf (stderr, "Unacceptable error\n");
      exit(-1);
    }
  }

  double expect1 = 1;
  double expect2 = 1;

  if (fft::get_normalization() == fft::nfft) {
    expect2 = double(ntrans)*double(ntrans);
    expect1 = ntrans;
  }

  test ("1", power(ntrans*2, fft1) / powerin, expect1);
  test ("2", power(ntrans*2, fft2) / powerin, expect2);

  // Now try taking a sub-band...  although the power should be
  // divided evenly into each channel, perform the test for each

  int ntests = 4;
  int nchan[4] = {8, 16, 32, 64};

  for (int j=0; j<ntests; j++) {
    fprintf (stderr, "\nForward:%dpt followed by %d sub-channels: \n",
	     ntrans, nchan[j]);

    int bdat = ntrans / nchan[j];
    fprintf (stderr, "Backward C->C FFT:%d\n", bdat);

    double powerout = 0.0;
    for (int ichan=0; ichan<nchan[j]; ichan++) {

      fft::bcc1d (bdat, fft2, fft1+ichan*bdat*2);
      // find the average multiplicative factor
      double chan_power = power (bdat*2, fft2);
      powerout += chan_power;
    }

    double factor2 = powerout/powerin;

    expect1 = double(ntrans)*double(bdat);
    expect2 = double(ntrans)/double(bdat);

    fprintf (stderr, "%d X %d = %lf\n", ntrans, bdat, expect1);
    fprintf (stderr, "%d / %d = %lf\n", ntrans, bdat, expect2);

    test ("3", powerout / powerin, expect1);

  }

}
