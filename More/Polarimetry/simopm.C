/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*

  Simulate polarized noise

*/

#include "Minkowski.h"
#include "Stokes.h"
#include "Jones.h"
#include "Pauli.h"
#include "Dirac.h"
#include "Jacobi.h"

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
    " -N Msamp    simulate Msamp mega samples \n"
    " -n Nint     integrate Nint samples before further processing \n"
    " -m Nsamp    box-car smooth over Nsamp samples before detection \n"
    " -M Nsamp    box-car smooth over Nsamp samples after detection \n"
    " -s i,q,u,v  single source with specified Stokes parameters \n"
    " -S          superposed modes \n"
    " -X          cross-correlate the modes after detection \n"
    " -C f_A      composite modes with fraction of instances in mode A \n"
    " -D F_A      disjoint modes with fraction of samples in mode A \n"
    " -A i,q,u,v  set the Stokes parameters of mode A \n"
    " -B i,q,u,v  set the Stokes parameters of mode B \n"
    " -l sigma    modulate mode A using a log-normal variate \n"
    " -a Nsamp    box-car smooth the amplitude modulation function \n"
    " -p          print real part of x and y, plus diff. phase \n"
    " -r          print the statistics of the coherency matrix \n"
    " -d          print only the variances of each Stokes parameter \n"
       << endl;
}

/***************************************************************************
 *
 *  spinor class represents an instance of the electric field
 *
 ***************************************************************************/

template<typename T>
class spinor
{
public:
  spinor (const std::complex<T>& _x, const std::complex<T>& _y) : x(_x), y(_y){}
  spinor () { }

  std::complex<T> x;
  std::complex<T> y;

  const spinor& operator *= (T scale) { x *= scale; y *= scale; return *this; }
  const spinor& operator /= (T norm) { x /= norm; y /= norm; return *this; }
  const spinor& operator += (const spinor& e) { x+=e.x; y+=e.y; return *this; }
};

template <typename T>
ostream& operator << (ostream& os, const spinor<T>& s)
{
  os << s.x << " " << s.y;
  return os;
}

template <typename T>
const spinor<T> operator + (spinor<T> s, const spinor<T>& t)
{
  return s += t;
}

//! returns the input spinor transformed by the Jones matrix
template<typename T>
const spinor<T> operator * (const Jones<T>& j, const spinor<T>& in)
{
  return spinor<T> ( j.j00 * in.x + j.j01 * in.y,
		     j.j10 * in.x + j.j11 * in.y );
}

template<typename T>
const spinor<T> operator * (double a, spinor<T> in)
{
  return in *= a;
}

template<typename T>
const spinor<T> operator * (spinor<T> in, double a)
{
  return in *= a;
}

/***************************************************************************
 *
 *  a single source of electromagnetic radiation
 *
 ***************************************************************************/

class mode
{
public:
  mode ();
  virtual ~mode () { }

  virtual void set_Stokes (const Stokes<double>& mean);
  virtual Stokes<double> get_Stokes () { return mean; }

  virtual Matrix<4,4, double> get_covariance ()
  { return Minkowski::outer (mean, mean); }
  virtual Stokes<double> get_mean () { return mean; }

  virtual spinor<double> get_field ();
  virtual BoxMuller* get_normal () { return normal; }
  virtual void set_normal (BoxMuller* n) { normal = n; }

  void set_power_law (double _inner_scale) { inner_scale = _inner_scale; }
  void powerlaw (spinor<double>&);

private:
  Stokes<double> mean;
  Jones<double> polarizer;

  BoxMuller* normal;
  double rms;

  double inner_scale;
};
  
mode::mode ()
{
  normal = 0;
  rms = 0.5;

  inner_scale = 0.0;

  set_Stokes (Stokes<double>(1.0));
}

void mode::set_Stokes (const Stokes<double>& _mean)
{
  mean = _mean;

  Quaternion<double,Hermitian> root = sqrt (natural(mean));
  polarizer = convert (root);
}

spinor<double> mode::get_field ()
{
  if (!normal)
    throw Error (InvalidState, "mode::get_field", "BoxMuller not set");

  BoxMuller& gasdev = *normal;

  complex<double> x (rms * gasdev(), rms * gasdev());
  complex<double> y (rms * gasdev(), rms * gasdev());

  spinor<double> e (x, y);

  if (inner_scale)
    powerlaw (e);

  return polarizer * e;
}

void mode::powerlaw (spinor<double>& e)
{
  double y = random_double(); 
  double index = -4;

  e *= pow( pow(inner_scale,index+1) * (1.0 - y), 1.0/(index+1) );
}

template<typename T, typename U>
void compute_stokes (Vector<4,T>& stokes, const spinor<U>& e)
{
  double var_x = norm(e.x);
  double var_y = norm(e.y);

  complex<double> c_xy = conj(e.x) * e.y;
  
  stokes[0] = var_x + var_y;
  stokes[1] = var_x - var_y;
  stokes[2] = 2.0*c_xy.real();
  stokes[3] = 2.0*c_xy.imag();
}

class mode_decorator : public mode
{
protected:
  mode* source;

public:
  mode_decorator (mode* s) { source = s; }
  mode* get_source () { return source; }

  void set_Stokes (const Stokes<double>& mean) { source->set_Stokes(mean); }
  Stokes<double> get_Stokes () { return source->get_Stokes(); }

  Matrix<4,4, double> get_covariance () { return source->get_covariance(); }
  Stokes<double> get_mean () { return source->get_mean(); }

  spinor<double> get_field () { return source->get_field(); }
  BoxMuller* get_normal () { return source->get_normal(); }
  void set_normal (BoxMuller* n) { source->set_normal(n); }
};

/***************************************************************************
 *
 *  a boxcar-smoothed mode of electromagnetic radiation
 *
 ***************************************************************************/

class boxcar_mode : public mode_decorator
{
  vector< spinor<double> > instances;
  unsigned smooth;
  unsigned current;

  void setup()
  {
    current = 0;
    instances.resize (smooth);
    for (unsigned i=1; i<smooth; i++)
      instances[i] = source->get_field();
  }

public:

  boxcar_mode (mode* s, unsigned n) : mode_decorator(s) { smooth = n; }

  spinor<double> get_field ()
  {
    if (instances.size() < smooth)
      setup ();

    instances[current] = source->get_field();
    current = (current + 1) % smooth;

    spinor<double> result;
    for (unsigned i=0; i<smooth; i++)
      result += instances[i];

    result /= sqrt(smooth);
    return result;
  }
};

/***************************************************************************
 *
 *  an amplitude modulated mode of electromagnetic radiation
 *
 ***************************************************************************/

class modulated_mode : public mode_decorator
{
  double tot, totsq;
  uint64_t count;

public:

  modulated_mode (mode* s) : mode_decorator(s) { tot=0; totsq=0; count=0; }

  ~modulated_mode ()
  {
    tot /= count;
    totsq /= count;
    totsq -= tot*tot;
    cerr << "modulated_mode mean=" << tot << " var=" << totsq << endl;
  }

  // return a random scalar modulation factor
  virtual double modulation () = 0;

  // return the mean of the scalar modulation factor
  virtual double get_mod_mean () = 0;

  // return the variance of the scalar modulation factor
  virtual double get_mod_variance () = 0;

  spinor<double> get_field ()
  {
    double mod = modulation();
    tot+=mod;
    totsq+=mod*mod;
    count+=1;
    return sqrt(mod) * source->get_field();
  }

  Matrix<4,4,double> get_covariance ()
  {
    double mean = get_mod_mean();
    double var = get_mod_variance();

    Matrix<4,4,double> C = source->get_covariance();
    C *= (mean*mean + var);
    Matrix<4,4,double> o = outer (get_Stokes(), get_Stokes());
    o *= var;
    return C + o;
  }

  Stokes<double> get_mean ()
  {
    return get_mod_mean () * source->get_mean();
  }

};

class lognormal_mode : public modulated_mode
{
  // standard deviation of the logarithm of the random variate
  double log_sigma;

public:

  lognormal_mode (mode* s, double ls) : modulated_mode (s) { log_sigma = ls;}

  // return a random scalar modulation factor
  double modulation ()
  {
    return exp ( log_sigma * (get_normal()->evaluate() - log_sigma) ) ;
  }

  double get_mod_mean () { return exp (-0.5*log_sigma*log_sigma); }
  
  double get_mod_variance () { return 1.0 - exp(-log_sigma*log_sigma); }

};


class boxcar_modulated_mode : public modulated_mode
{
  vector< double > instances;
  unsigned smooth;
  unsigned current;

  void setup()
  {
    current = 0;
    instances.resize (smooth);
    for (unsigned i=1; i<smooth; i++)
      instances[i] = mod->modulation();
  }

  modulated_mode* mod;

  unsigned count;

public:

  boxcar_modulated_mode (modulated_mode* s, unsigned n)
    : modulated_mode(s->get_source()) { smooth = n; mod = s; count = 0; }

  double modulation ()
  {
    if (instances.size() < smooth)
      setup ();

    instances[current] = mod->modulation();
    current = (current + 1) % smooth;

    double result;
    for (unsigned i=0; i<smooth; i++)
      result += instances[i];

    result /= smooth;
#if 0
    if (count < 1024*128)
    {
      cerr << "mod " <<  result << endl;
      count ++;
    }
#endif
    return result;
  }

  double get_mod_variance ()
  {
    return mod->get_mod_variance() / smooth;
  }

  double get_mod_mean ()
  {
    return mod->get_mod_mean () / smooth;
  }
};



/***************************************************************************
 *
 *  a Stokes sample
 *
 ***************************************************************************/

class sample
{
public:

  unsigned sample_size;

  sample() { sample_size = 1; }

  virtual ~sample () {}

  virtual Stokes<double> get_Stokes () = 0;
  virtual Vector<4, double> get_mean () = 0;
  virtual Matrix<4,4, double> get_covariance () = 0;
};

/***************************************************************************
 *
 *  a single source of electromagnetic radiation
 *
 ***************************************************************************/

class single : public sample
{
public:
  mode* source;

  single (mode* s) { source = s; }

  ~single () { delete source; }

  virtual Stokes<double> get_Stokes_instance ()
  {
    spinor<double> e = source->get_field();
    Vector<4, double> tmp;
    compute_stokes (tmp, e);
    return tmp;
  }

  Stokes<double> get_Stokes ()
  {
    Stokes<double> result;
    for (unsigned i=0; i<sample_size; i++)
      result += get_Stokes_instance();
    result /= sample_size;
    return result;
  }

  Vector<4, double> get_mean ()
  {
    return source->get_mean();
  }

  Matrix<4,4, double> get_covariance ()
  {
    Matrix<4,4, double> result = source->get_covariance ();
    result /= sample_size;
    return result;
  }
};


/***************************************************************************
 *
 *  a combination of two sources of electromagnetic radiation
 *
 ***************************************************************************/

class combination : public sample
{
public:
  mode A;
  mode B;

  void set_normal (BoxMuller* n) { A.set_normal(n); B.set_normal(n); }
};

/***************************************************************************
 *
 *  a superposition of two sources of electromagnetic radiation
 *
 ***************************************************************************/

class superposed : public combination
{
  Stokes<double> get_Stokes ()
  {
    Stokes<double> result;
    for (unsigned i=0; i<sample_size; i++)
    {
      spinor<double> e_A = A.get_field();
      spinor<double> e_B = B.get_field();

      Vector<4, double> tmp;
      compute_stokes (tmp, e_A + e_B);
      result += tmp;
    }
    result /= sample_size;
    return result;
  }

  Vector<4, double> get_mean ()
  {
    return A.get_mean() + B.get_mean();
  }

  Matrix<4,4, double> get_covariance ()
  {
    Stokes<double> stokes = A.get_Stokes() + B.get_Stokes();
    Matrix<4,4, double> result = Minkowski::outer(stokes, stokes);
    result /= sample_size;
    return result;
  }
};

/***************************************************************************
 *
 *  a composition of two sources of electromagnetic radiation
 *
 ***************************************************************************/

class composite : public combination
{
  double A_fraction;

public:

  composite (double fraction) { A_fraction = fraction; }

  Stokes<double> get_Stokes ()
  {
    unsigned A_sample_size = A_fraction * sample_size;

    Stokes<double> result;

    for (unsigned i=0; i<sample_size; i++)
    {
      spinor<double> e;
      if (i < A_sample_size)
	e = A.get_field();
      else
	e = B.get_field();

      Vector<4, double> tmp;
      compute_stokes (tmp, e);
      result += tmp;
    }

    result /= sample_size;
    return result;
  }

  Vector<4, double> get_mean ()
  {
    unsigned A_sample_size = A_fraction * sample_size;
    unsigned B_sample_size = sample_size - A_sample_size;
    Vector<4,double> result 
      = A_sample_size * A.get_mean()
      + B_sample_size * B.get_mean();
    result /= sample_size;
    return result;
  }

  Matrix<4,4, double> get_covariance ()
  {
    unsigned A_sample_size = A_fraction * sample_size;
    unsigned B_sample_size = sample_size - A_sample_size;

    Matrix<4,4,double> C_A = Minkowski::outer (A.get_Stokes(), A.get_Stokes());
    Matrix<4,4,double> C_B = Minkowski::outer (B.get_Stokes(), B.get_Stokes());

    C_A *= A_sample_size / double(sample_size * sample_size);
    C_B *= B_sample_size / double(sample_size * sample_size);

    return C_A + C_B;
  }
};

/***************************************************************************
 *
 *  a disjoint combination of two sources of electromagnetic radiation
 *
 ***************************************************************************/

class disjoint : public combination
{
  double A_fraction;

public:

  disjoint (double fraction) { A_fraction = fraction; }

  Stokes<double> get_Stokes ()
  {
    bool mode_A = random_double() < A_fraction;
    mode* e = (mode_A) ? &A : &B;

    Stokes<double> result;

    for (unsigned i=0; i<sample_size; i++)
    {
      Vector<4, double> tmp;
      compute_stokes (tmp, e->get_field());
      result += tmp;
    }

    result /= sample_size;
    return result;
  }

  Vector<4, double> get_mean ()
  {
    return A_fraction * A.get_mean() + (1-A_fraction) * B.get_mean();
  }

  Matrix<4,4, double> get_covariance ()
  {
    Matrix<4,4,double> C_A = Minkowski::outer (A.get_Stokes(), A.get_Stokes());
    Matrix<4,4,double> C_B = Minkowski::outer (B.get_Stokes(), B.get_Stokes());
    Vector<4,double> diff = A.get_Stokes() - B.get_Stokes();
    Matrix<4,4,double> D = outer (diff, diff);

    C_A *= A_fraction / sample_size;
    C_B *= (1-A_fraction) / sample_size;
    D *= A_fraction * (1-A_fraction);

    return C_A + C_B + D;
  }
};


/***************************************************************************
 *
 *  a post-detection boxcar-smoothed source of electromagnetic radiation
 *
 ***************************************************************************/

class boxcar_sample : public single
{
  vector< Stokes<double> > instances;
  unsigned smooth;
  unsigned current;

  void setup()
  {
    current = 0;
    instances.resize (smooth);
    for (unsigned i=1; i<smooth; i++)
      instances[i] = single::get_Stokes_instance();
  }

public:

  boxcar_sample (mode* s, unsigned n) : single(s) { smooth = n; }

  Stokes<double> get_Stokes_instance ()
  {
    if (instances.size() < smooth)
      setup ();

    instances[current] = single::get_Stokes_instance();
    current = (current + 1) % smooth;

    Stokes<double> result;
    for (unsigned i=0; i<smooth; i++)
      result += instances[i];

    result /= smooth;
    return result;
  }
};



int main (int argc, char** argv)
{
  uint64_t ndat = 1024 * 1024;     // number of Stokes samples
  unsigned nint = 1;               // number of instances in Stokes sample

  unsigned smooth_before = 0;      // box-car smoothing width pre-detection
  unsigned smooth_after = 0;       // box-car smoothing width post-detection
  unsigned smooth_modulator = 0;

  bool verbose = false;

  Stokes<double> stokes = 1.0;
  bool subtract_outer_population_mean = false;

  mode source;
  combination* dual = NULL;
  sample* stokes_sample = NULL;

  bool cross_correlate = false;

  bool print = false;
  bool rho_stats = false;
  bool variances_only = false;

  double log_sigma = 0.0;

  int c;
  while ((c = getopt(argc, argv, "a:dhn:N:m:M:s:SC:D:A:B:l:oprX")) != -1)
  {
    switch (c)
    {

    case 'h':
      usage ();
      return 0;

    case 's':
    case 'A':
    case 'B':
    {
      double i,q,u,v;
      if (sscanf (optarg, "%lf,%lf,%lf,%lf", &i,&q,&u,&v) != 4)
      {
	cerr << "Error parsing " << optarg << " as Stokes 4-vector" << endl;
	return -1;
      }
      stokes = Stokes<double> (i,q,u,v);

      if (stokes.abs_vect() > i) {
	cerr << "Invalid Stokes parameters (p>I) " << stokes << endl;
	return -1;
      }

      if (dual && c=='A')
	dual->A.set_Stokes( stokes );
      if (dual && c=='B')
	dual->B.set_Stokes( stokes );

      break;
    }

    case 'l':
      log_sigma = atof (optarg);
      break;
      
    case 'N':
      ndat = ndat * atof (optarg);
      break;

    case 'n':
      nint = atoi (optarg);
      break;

    case 'M':
      smooth_after = atoi (optarg);
      break;

    case 'm':
      smooth_before = atoi (optarg);
      break;

    case 'a':
      smooth_modulator = atoi (optarg);
      break;

    case 'o':
      subtract_outer_population_mean = true;
      break;

    case 'p':
      print = true;
      break;

    case 'S':
      dual = new superposed;
      break;

    case 'X':
      dual = new superposed;
      cross_correlate = true;
      break;

    case 'C':
      dual = new composite( atof(optarg) );
      break;

    case 'D':
      dual = new disjoint( atof(optarg) );
      break;

    case 'r':
      rho_stats = true;
      break;

    case 'd':
      variances_only = true;
      break;

    case 'v':
      verbose = true;
      break;
    }
  }

  if ((smooth_before || smooth_after) && dual)
    {
      cerr << "Cannot currently box-car smooth when combining modes" << endl;
      return -1;
    }

  cerr << "Simulating " << ndat << " Stokes samples" << endl;

  random_init ();
  BoxMuller gasdev (time(NULL));

  uint64_t ntot = 0;
  Vector<4, double> tot;
  Matrix<4,4, double> totsq;

  Matrix<2,2, complex<double> > tot_rho;
  Matrix<4,4, complex<double> > totsq_rho;

  source.set_Stokes (stokes);

  if (dual)
  {
    dual->set_normal (&gasdev);
    stokes_sample = dual;
  }
  else
  {
    source.set_normal (&gasdev);

    mode* s = &source;

    modulated_mode* mod = 0;

    if (log_sigma)
      s = mod = new lognormal_mode (s, log_sigma);

    if (smooth_modulator > 1 && mod)
      s = new boxcar_modulated_mode (mod, smooth_modulator);

    if (smooth_before > 1)
      s = new boxcar_mode (s, smooth_before);

    if (smooth_after > 1)
      stokes_sample = new boxcar_sample (s, smooth_after);
    else
      stokes_sample = new single(s);
  }

  stokes_sample->sample_size = nint;

  if (cross_correlate)
  {
    sample* A = new single (&(dual->A));
    A->sample_size = nint;

    sample* B = new single (&(dual->B));
    B->sample_size = nint;

    Vector<4, double> tot_A;
    Vector<4, double> tot_B;
    Matrix<4,4, double> tot_AB;

    for (uint64_t idat=0; idat<ndat; idat++)
    {
      Vector<4, double> S_A = A->get_Stokes();
      Vector<4, double> S_B = B->get_Stokes();

      tot_A += S_A;
      tot_B += S_B;
      tot_AB += outer(S_A, S_B);

      ntot ++;
    }

    tot_A /= ntot;
    tot_B /= ntot;
    tot_AB /= ntot;
    tot_AB -= outer(tot_A,tot_B);

    cerr << "\nAmean=" << tot_A << endl;
    cerr << "\nBmean=" << tot_B << endl;
    cerr << "\nXcovar=\n" << tot_AB << endl;
    return 0;
  }

  for (uint64_t idat=0; idat<ndat; idat++)
  {
    Vector<4, double> mean_stokes;

    mean_stokes = stokes_sample->get_Stokes();

    tot += mean_stokes;
    totsq += outer(mean_stokes, mean_stokes);

    if (rho_stats)
    {
      Matrix<2,2, complex<double> > rho = convert (Stokes<double>(mean_stokes));
    
      tot_rho += rho;
      totsq_rho += direct (rho, rho);
    }

    ntot ++;
  }

  tot /= ntot;
  totsq /= ntot;

  if (subtract_outer_population_mean)
    totsq -= outer(stokes,stokes);
  else
    totsq -= outer(tot,tot);

  if (variances_only)
  {
    for (unsigned i=0; i<4; i++)
      cout << "var[" << i << "] = " << totsq[i][i] << endl;
    
    return 0;
  }

  Vector<4, double> expected_mean;
  Matrix<4,4, double> expected_covariance;

  expected_mean = stokes_sample->get_mean ();
  expected_covariance = stokes_sample->get_covariance();

  cerr << "\n"
    " ******************************************************************* \n"
    "\n"
    " STOKES PARAMETERS \n"
    "\n"
    " ******************************************************************* \n"
       << endl;

  cerr << "mean=" << tot << endl;
  cerr << "expected=" << expected_mean << endl;

  cerr << "\ncovar=\n" << totsq << endl;
  cerr << "expected=\n" << expected_covariance << endl;

  delete stokes_sample;

  if (!rho_stats)
    return 0;

  cerr << "\n"
    " ******************************************************************* \n"
    "\n"
    " COHERENCY MATRIX \n"
    "\n"
    " ******************************************************************* \n"
       << endl;

  tot_rho /= ndat;
  totsq_rho /= ndat;

  cerr << "rho sq=\n" << totsq_rho << endl;

  totsq_rho -= direct(tot_rho,tot_rho);

  cerr << "rho mean=\n" << tot_rho << endl;
  cerr << "rho covar=\n" << totsq_rho << endl;

  Matrix<4,4, complex<double> > candidate;
  for (unsigned i=0; i<4; i++)
    for (unsigned j=0; j<4; j++)
      {
	Matrix<4,4,complex<double> > temp = Dirac::matrix (i,j);
	temp *= expected_covariance[i][j] * 0.25;

	candidate += temp;
      }

  cerr << "candidate=\n" << candidate << endl;

  Matrix<4, 4, complex<double> > eigenvectors;
  Vector<4, double> eigenvalues;

  Matrix<4, 4, complex<double> > temp = candidate;
  Jacobi (temp, eigenvectors, eigenvalues);

  for (unsigned i=0; i<4; i++)
    cerr << "e_" << i << "=" << eigenvalues[i] << "  v=" << eigenvectors[i] << endl;

  return 0;
}

