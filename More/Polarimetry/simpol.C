/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*

  Simulate polarized noise with optional digitization

*/

#include <iostream>
#include <unistd.h>

using namespace std;

#include "Stokes.h"
#include "Jones.h"
#include "Pauli.h"

#include "NormalDistribution.h"
#include "BoxMuller.h"
#include "environ.h"

void usage ()
{
  cout <<

    "simpol: simulate polarized noise and compute statistics \n" 
    " \n"
    "options: \n"
    " \n"
    " -b nbit     digitize using nbit bits \n"
    " -n Msamp    simulate Msamp mega samples \n"
    " -s i,q,u,v  specify the Stokes parameters of the input signal \n"

       << endl;
}

void transform (Jones<float>& jones, float* ex, float* ey)
{
  complex<float> x (ex[0], ex[1]);
  complex<float> y (ey[0], ey[1]);

  complex<float> t;

  t = jones(0,0) * x + jones(0,1) * y;
  ex[0] = t.real();
  ex[1] = t.imag();

  t = jones(1,0) * x + jones(1,1) * y;
  ey[0] = t.real();
  ey[1] = t.imag();
}

bool unity_variance = false;

void digitize (float& volts, float scale, float max, float rescale)
{
  float value = (0.5 + std::min(max, floor(fabs(volts) * scale)));

  if (unity_variance)
    value *= rescale;

  if (volts < 0)
    volts = -value;
  else
    volts = value;
}

int main (int argc, char** argv)
{
  uint64 ndat = 1024 * 1024;       // 1Mpt set of random samples
  unsigned nbit = 0;               // number of bits per sample in digitizer

  bool maintain_thresholds = true; // do not reset sampling thresholds
  bool polarized = false;

  Stokes<float> stokes (1,0,0,0);

  bool verbose = false;
  int c;
  while ((c = getopt(argc, argv, "hb:n:s:")) != -1) {
    switch (c)  {

    case 'h':
      usage ();
      return 0;

    case 'b':
      nbit = atoi (optarg);
      break;

    case 's': {
      float i,q,u,v;
      if (sscanf (optarg, "%f,%f,%f,%f", &i,&q,&u,&v) != 4) {
	cerr << "Error parsing " << optarg << " as Stokes 4-vector" << endl;
	return -1;
      }
      polarized = true;
      stokes = Stokes<float> (i,q,u,v);

      if (stokes.abs_vect() > i) {
	cerr << "Invalid Stokes parameters (p>I) " << stokes << endl;
	return -1;
      }

      break;
    }
      
    case 'n':
      ndat *= atoi (optarg);
      break;

    case 'v':
      verbose = true;
      break;
    }
  }

  cerr << "Simulating " << ndat << " samples ..." << endl;

  Quaternion<float,Hermitian> root = sqrt (natural(stokes));
  Jones<float> polarizer = convert (root);

  double input_rms = 0.5;

  double expected_rms0 = input_rms;
  double expected_rms1 = input_rms;

  if (!maintain_thresholds)
  {
    expected_rms0 *= abs( polarizer(0,0) );
    expected_rms1 *= abs( polarizer(1,1) );
  }

  double scale0 = 0.0;
  double scale1 = 0.0;
  double rescale = 0.0;
  float saturation = 0.0;

  if (nbit)
  {
    assert (nbit > 1 && nbit < 9);

    static double spacing [9] = { 0, 0,  // 0 and 1 bit not handled
				  0.9674,
				  0.5605,
				  0.3188,
				  0.1789,
				  0.09925,
				  0.05445,
				  0.02957 };

    float the_spacing = spacing[nbit];

    saturation = pow(2.0,double(nbit-1)) - 1.0;
    cerr << "ditigiter saturates at " << saturation << endl;

    NormalDistribution normal;
    double cumulative_probability = 0.0;
    double variance = 0.0;

    unsigned imax = (saturation+1);
    for (unsigned i=0; i<imax; i++)
    {
      double output = 0.5 + float (i);
      double interval = 0.0;

      if (i+1 < imax)
      {
	double threshold = double(i+1) * the_spacing;
	double cumulative = normal.cumulative_distribution (threshold) - 0.5;
	interval = cumulative - cumulative_probability;
	cumulative_probability = cumulative;
      }
      else
	interval = 0.5 - cumulative_probability;

      // cerr << "i=" << i << " p=" << interval << " out=" << output << endl;
    
      variance += output*output * interval;

#ifdef _DEBUG
      cerr << i << " t=" << threshold << " c=" << interval 
           << " v=" << variance << endl;
#endif
    }

    // integrated over only half of the normal distribution
    variance *= 2.0;

    cerr << "variance=" << variance << endl;
    // want the rms in each of the voltages to be around unity
    rescale = 0.5/sqrt(variance);

    scale0 = 1.0/(the_spacing*expected_rms0);
    cerr << "x scale=" << scale0 << endl;

    scale1 = 1.0/(the_spacing*expected_rms1);
    cerr << "y scale=" << scale1 << endl;
  }

  float e_x [2];
  float e_y [2];

  BoxMuller gasdev (-1);

  Vector<4, double> tot;
  Matrix<4,4, double> totsq;

  for (uint64 idat=0; idat<ndat; idat++)
  {
    e_x[0] = input_rms * gasdev ();
    e_y[0] = input_rms * gasdev ();
    e_x[1] = input_rms * gasdev ();
    e_y[1] = input_rms * gasdev ();

    if (polarized)
      transform (polarizer, e_x, e_y);

    if (nbit)
    {
      digitize (e_x[0], scale0, saturation, rescale);
      digitize (e_x[1], scale0, saturation, rescale);
      digitize (e_y[0], scale1, saturation, rescale);
      digitize (e_y[1], scale1, saturation, rescale);
    }

    float ex_r = e_x[0];
    float ex_i = e_x[1];
    float ey_r = e_y[0];
    float ey_i = e_y[1];
 
    double var_x = ex_r * ex_r + ex_i * ex_i;
    double var_y = ey_r * ey_r + ey_i * ey_i;

    Vector<4, double> stokes;

    stokes[0] = var_x + var_y;
    stokes[1] = var_x - var_y;
    stokes[2] = 2.0*(ex_r * ey_r + ex_i * ey_i);
    stokes[3] = 2.0*(ex_r * ey_i - ex_i * ey_r);

    tot += stokes;
    totsq += outer(stokes, stokes);
  }

  tot /= ndat;
  totsq /= ndat;
  totsq -= outer(tot,tot);

  cerr << "mean=" << tot << endl;

  cerr << "covar=" << totsq << endl;

  return 0;
}

