/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*

  Simulate polarized noise with optional digitization

*/

#include "JenetAnderson98.h"
#include "Minkowski.h"
#include "Stokes.h"
#include "Jones.h"
#include "Pauli.h"
#include "Dirac.h"
#include "Jacobi.h"

#include "NormalDistribution.h"
#include "BoxMuller.h"
#include "random.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>

using namespace std;

void usage ()
{
  cout <<

    "simpol: simulate polarized noise and compute statistics \n" 
    " \n"
    "options: \n"
    " \n"
    " -b nbit     digitize using nbit bits \n"
    " -d space    spacing between digitizer levels \n"
    " -n Msamp    simulate Msamp mega samples \n"
    " -N Nint     integrate Nint samples before further processing \n"
    " -l factor   simulate non-linearity \n"
    " -L q,u,v    simulate log-normal statistics in specified basis \n"
    " -f gain     simulate digitizer sign-bit feedback \n"
    " -o          every other sample in orthogonally polarized mode (OPM) \n"
    " -p rad      apply differential phase (in radians) to input \n"
    " -P min      simulate a power law distribution over min to infinity \n"
    " -s i,q,u,v  specify the Stokes parameters of the input signal \n"
       << endl;
}

void transform (Jones<double>& jones, double* ex, double* ey)
{
  complex<double> x (ex[0], ex[1]);
  complex<double> y (ey[0], ey[1]);

  complex<double> t;

  t = jones(0,0) * x + jones(0,1) * y;
  ex[0] = t.real();
  ex[1] = t.imag();

  t = jones(1,0) * x + jones(1,1) * y;
  ey[0] = t.real();
  ey[1] = t.imag();
}

void compute_stokes (Vector<4,double>& stokes,
		     const complex<double>& ex,
		     const complex<double>& ey)
{
  double var_x = norm(ex);
  double var_y = norm(ey);

  complex<double> c_xy = ex * conj(ey);
  
  stokes[0] = var_x + var_y;
  stokes[1] = var_x - var_y;
  stokes[2] = 2.0*c_xy.real();
  stokes[3] = 2.0*c_xy.imag();
}

void nonlinear (double& value, double factor)
{
  value = asinh (factor * value) / factor;
}

double sign (double x)
{
  if (x > 0)
    return 1.0;
  else
    return -1.0;
}

void feedback_sign (double& value, double gain)
{
  value += gain * sign(value);
}

void digitize (double& volts, double scale, double max, double rescale)
{
  double value = (0.5 + std::min(max, floor(fabs(volts) * scale)));

  value *= rescale;

  if (volts < 0)
    volts = -value;
  else
    volts = value;
}

void powerlaw_polar (double* v, double min)
{
  double y = random_double(); 

  double index = -4;
  double r = pow( pow(min,index+1) * (1.0 - y), 1.0/(index+1) );

  // cout << y << " " << r << endl;

  double theta = 0;
  random_value (theta, M_PI);
  v[0] = r * cos(theta);
  v[1] = r * sin(theta);

  // cout << v[0] << " " << v[1] << endl;
}

void lognormal_polar (double* v, double sigma, BoxMuller& gasdev)
{
  double r = exp ( sigma * gasdev() );
  double theta = 0;
  random_value (theta, M_PI);
  v[0] = r * cos(theta);
  v[1] = r * sin(theta);

  // cout << v[0] << " " << v[1] << endl;
}

void uniform_polar (double* v)
{
  double r = random_double();
  double theta = 0;
  random_value (theta, M_PI);
  v[0] = r * cos(theta);
  v[1] = r * sin(theta);

  // cout << v[0] << " " << v[1] << endl;
}


template<typename T>
class spinor {
public:
  spinor (const std::complex<T>& _x, const std::complex<T>& _y) : x(_x), y(_y){}
  
  std::complex<T> x;
  std::complex<T> y;
};

//! Returns the inverse
template<typename T>
spinor<T> operator * (const Jones<T>& j, const spinor<T>& in)
{
  return spinor<T> ( j.j00 * in.x + j.j01 * in.y,
		     j.j10 * in.x + j.j11 * in.y );
}

int main (int argc, char** argv)
{
  uint64_t ndat = 1024 * 1024;       // 1Mpt set of random samples

  unsigned nint = 0;
  unsigned jint = 0;

  unsigned nbit = 0;               // number of bits per sample in digitizer
  double digitizer_spacing = 0;    // spacing between digitizer levels

  bool maintain_thresholds = true; // do not reset sampling thresholds
  bool polarized = false;
  bool opm = false;
  double opm_coherence = 0.0;

  bool lognormal = false;
  Jones<double> logbasis;
  double ln_sigma = 0.5;

  bool uniform = false;
  Jones<double> unibasis;

  double power_law = 0.0;

  Stokes<double> stokes (1,0,0,0);
  double phase = 0;

  double non_linearity = 0.0;
  double adc_feedback = 0.0;
  
  bool verbose = false;

  int c;
  while ((c = getopt(argc, argv, "hb:c:d:f:l:L:n:N:op:P:s:U")) != -1) {
    switch (c)  {

    case 'h':
      usage ();
      return 0;

    case 'b':
      nbit = atoi (optarg);
      break;

    case 'c':
      opm_coherence = atof (optarg);
      opm = true;
      break;

    case 'd':
      digitizer_spacing = atof (optarg);
      break;

    case 'f':
      adc_feedback = atof (optarg);
      cerr << "simpol: digitizer feedback gain=" << adc_feedback << endl;
      break;

    case 's': {
      double i,q,u,v;
      if (sscanf (optarg, "%lf,%lf,%lf,%lf", &i,&q,&u,&v) != 4)
      {
	cerr << "Error parsing " << optarg << " as Stokes 4-vector" << endl;
	return -1;
      }
      polarized = true;
      stokes = Stokes<double> (i,q,u,v);

      if (stokes.abs_vect() > i) {
	cerr << "Invalid Stokes parameters (p>I) " << stokes << endl;
	return -1;
      }

      cerr << "Input mean Stokes = " << stokes << endl;
      break;
    }

    case 'l':
      non_linearity = atof (optarg);
      cerr << "simpol: non-linearity factor=" << non_linearity << endl;
      break;

    case 'L':
    {
      double q,u,v;
      if (sscanf (optarg, "%lf,%lf,%lf", &q,&u,&v) != 3)
      {
	cerr << "Error parsing " << optarg << " as Stokes 3-vector" << endl;
	return -1;
      }

      Vector<3,double> vect;
      vect[0] = q;
      vect[1] = u;
      vect[2] = v;
      
      Quaternion<double,Hermitian> rho (norm(vect), vect);
      logbasis = convert (eigen(rho));

      lognormal = true;

      cerr << logbasis << endl;

      cerr << "simpol: lognormal electric field amplitude" << endl;
      break;
    }

    case 'n':
      ndat = ndat * atof (optarg);
      break;

    case 'N':
      nint = atoi (optarg);
      break;

    case 'o':
      opm = true;
      break;

    case 'p':
      phase = atof (optarg);
      break;

    case 'P':
      power_law = atof (optarg);
      cerr << "simpol: powerlaw min = " << power_law << endl;
      break;

    case 'U':
      cerr << "simpol: uniformly distributed" << endl;
      uniform = true;
      break;

    case 'v':
      verbose = true;
      break;
    }
  }

  if (phase)
  {
    double x = stokes[2];
    double y = stokes[3];
    double cp = cos(phase);
    double sp = sin(phase);

    stokes[2] = cp*x - sp*y;
    stokes[3] = sp*x + cp*y;

    cerr << "Phase rotated stokes: " << stokes << endl;
  }

  cerr << "Simulating " << ndat << " samples ..." << endl;

  Quaternion<double,Hermitian> root = sqrt (natural(stokes));
  Jones<double> polarizer = convert (root);

  random_init ();
  BoxMuller gasdev (time(NULL));

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
  double saturation = 0.0;

  if (nbit)
  {
    if (digitizer_spacing == 0.0)
      digitizer_spacing = JenetAnderson98::get_optimal_spacing (nbit);

    saturation = pow(2.0,double(nbit-1)) - 1.0;
    cerr << "ditigiter saturates at " << saturation << endl;

    NormalDistribution normal;
    double cumulative_probability = 0.0;
    double variance = 0.0;

    unsigned imax = (saturation+1);
    for (unsigned i=0; i<imax; i++)
    {
      double output = 0.5 + double (i);
      double interval = 0.0;

      if (i+1 < imax)
      {
	double threshold = double(i+1) * digitizer_spacing;
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

    scale0 = 1.0/(digitizer_spacing*expected_rms0);
    cerr << "x scale=" << scale0 << endl;

    scale1 = 1.0/(digitizer_spacing*expected_rms1);
    cerr << "y scale=" << scale1 << endl;
  }

  double e_x [2];
  double e_y [2];

  Vector<4, double> tot_stokes;
  uint64_t ntot = 0;
  Vector<4, double> tot;
  Matrix<4,4, double> totsq;

  Matrix<2,2, complex<double> > tot_rho;
  Matrix<4,4, complex<double> > totsq_rho;

  Vector<3, complex<double> > tot_3;
  Matrix<3,3, complex<double> > totsq_3;

  complex<double> last_ex;
  complex<double> last_ey;

  for (uint64_t idat=0; idat<ndat; idat++)
  {
    if (uniform)
    {
      uniform_polar (e_x);
      uniform_polar (e_y);
    }
    else if (power_law)
    {
      powerlaw_polar (e_x, power_law);
      powerlaw_polar (e_y, power_law);
    }
    else if (lognormal)
    {
      lognormal_polar (e_x, ln_sigma, gasdev);
      lognormal_polar (e_y, ln_sigma, gasdev);

      transform (logbasis, e_x, e_y);
    }
    else
    {
      e_x[0] = input_rms * gasdev ();
      e_y[0] = input_rms * gasdev ();
      e_x[1] = input_rms * gasdev ();
      e_y[1] = input_rms * gasdev ();
    }

    if (polarized)
      transform (polarizer, e_x, e_y);

    if (non_linearity)
    {
      nonlinear (e_x[0], non_linearity);
      nonlinear (e_y[0], non_linearity);
      nonlinear (e_x[1], non_linearity);
      nonlinear (e_y[1], non_linearity);
    }

    if (adc_feedback)
    {
      feedback_sign (e_x[0], adc_feedback);
      feedback_sign (e_y[0], adc_feedback);
      feedback_sign (e_x[1], adc_feedback);
      feedback_sign (e_y[1], adc_feedback);
    }

    if (nbit)
    {
      digitize (e_x[0], scale0, saturation, rescale);
      digitize (e_x[1], scale0, saturation, rescale);
      digitize (e_y[0], scale1, saturation, rescale);
      digitize (e_y[1], scale1, saturation, rescale);
    }

    complex<double> ex (e_x[0], e_x[1]);
    complex<double> ey (e_y[0], e_y[1]);
 
    if (opm_coherence)
    {
      if (idat % 2 == 0)
      {
	last_ex = ex;
	last_ey = ey;
	continue;
      }
      else
      {
	complex<double> e0x = last_ex + opm_coherence * ex;
	complex<double> e0y = last_ey + opm_coherence * ey;
	Vector<4, double> stokes0;
	compute_stokes (stokes0, e0x, e0y);

	if (!nint)
	{
	  tot += stokes0;
	  totsq += outer(stokes0, stokes0);
	  ntot ++;
	}

	complex<double> e1x = ex + opm_coherence * last_ex;
	complex<double> e1y = ey + opm_coherence * last_ey;	
	Vector<4, double> stokes1;
	compute_stokes (stokes1, e1x, e1y);
	for (unsigned i=1; i<4; i++)
	  stokes1[i] *= -1.0;

	if (!nint)
	{
	  tot += stokes1;
	  totsq += outer(stokes1, stokes1);
	  ntot ++;
	}

	if (nint == 2)
	{
	  Vector<4, double> stokes = stokes0 + stokes1;
	  tot += stokes;
	  totsq += outer(stokes, stokes);
	  ntot ++;
	}
      }
    }

    Vector<4, double> temp_stokes;
    compute_stokes (temp_stokes, ex, ey);

    if (opm && (idat % 2))
      for (unsigned i=1; i<4; i++)
	temp_stokes[i] *= -1.0;

    if (nint == 0)
      tot_stokes = temp_stokes;
    else
    {
      if (jint == 0)
	tot_stokes = temp_stokes;
      else
	tot_stokes += temp_stokes;

      jint ++;
      // cerr << jint << endl;

      if (jint < nint)
	continue;

      // cerr << "add" << endl;
      tot_stokes /= nint;
      jint = 0;
    }

    tot += tot_stokes;
    totsq += outer(tot_stokes, tot_stokes);
    ntot ++;
  }

  tot /= ntot;
  totsq /= ntot;
  totsq -= outer(tot,tot);

  cerr << "mean=" << tot << endl;

  if (phase)
    cerr << "phase=" << atan2(tot[3],tot[2]) << endl;

  cerr << "covar=\n" << totsq << endl;

  Matrix<4,4, double> expected = Minkowski::outer(stokes, stokes);

  if (opm)
    {
      Vector<4,double> modeB = - stokes;
      modeB[0] = stokes[0];

      Matrix<4,4, double> covB = Minkowski::outer(modeB, modeB);
      expected += covB;
      /*
      if (nint == 0)
	{
	  expected += outer(stokes,stokes);
	  expected += outer(modeB,modeB);
	}
      */
      expected /= 2;
      
      if (nint == 0)
	{
	  // subtract outer product of mean to produce covariance
	  Vector<4,double> mean = 0.5*(stokes-modeB);
	  expected += outer(mean,mean);
	}

    }

  if (nint)
    expected /= nint;

  cerr << "expected=\n" << expected << endl;

  return 0;

  tot_rho /= ndat;
  totsq_rho /= ndat;
  totsq_rho -= direct(tot_rho,tot_rho);

  cerr << "rho mean=\n" << tot_rho << endl;
  cerr << "rho covar=\n" << totsq_rho << endl;

  Matrix<4,4, complex<double> > candidate;
  for (unsigned i=0; i<4; i++)
    for (unsigned j=0; j<4; j++)
      {
	Matrix<4,4,complex<double> > temp = Dirac::matrix (i,j);
	temp *= expected[i][j] * 0.25;

	candidate += temp;
      }

  cerr << "candidate=\n" << candidate << endl;

  Matrix<4, 4, complex<double> > eigenvectors;
  Vector<4, double> eigenvalues;

  Matrix<4, 4, complex<double> > temp = candidate;
  Jacobi (temp, eigenvectors, eigenvalues);

  for (unsigned i=0; i<4; i++)
    cerr << "e_" << i << "=" << eigenvalues[i] << "  v=" << eigenvectors[i] << endl;







  {

  tot_3 /= ndat;
  totsq_3 /= ndat;


  totsq_3 -= outer(tot_3,conj(tot_3));

  cerr << "3 mean=\n" << tot_3 << endl;
  cerr << "3 covar=\n" << totsq_3 << endl;

  Matrix<3, 3, complex<double> > eigenvectors;
  Vector<3, double> eigenvalues;

  Matrix<3, 3, complex<double> > temp = totsq_3;
  Jacobi (temp, eigenvectors, eigenvalues);

  for (unsigned i=0; i<3; i++)
    cerr << "e_" << i << "=" << eigenvalues[i] << "  v=" << eigenvectors[i] << endl;

  }







  return 0;
}

