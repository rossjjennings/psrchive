#include <algorithm>
#include <math.h>
#include "Pulsar/Profile.h"
#include "fftm.h"

// redwards --- code for finding the phase shift w.r.t. a template profile,
// using sinc interpolation of the cross correlation function

unsigned Pulsar::Profile::SIS_zap_period = 0;



static float
sinc_interp(float *f, double x, int n)
{
  while (x < 0.0)
    x+= n;
  while (x >= n)
    x-= n;

  if (x == floor(x))
    return f[(int)floor(x)];
 
  // uses technique of Schanze, IEEE Trans Sig proc 42(6) 1502-1503 1995
  // for even n,
  //    f(x) = sin(pi.x)/pi sum_{i=-L}^{M-1} f[i] -1^i cot(pi.(x-i)/n)
  // where L+M=N.
  // We can (apparently?) choose L=0 to have M=N

  if (n%2)
  {
    fprintf(stderr, "Error, odd numbers of bins not implemented in sinc_interp!!\n"); // possible but uglier
    exit(1);
  }

  int i;
  double result = 0.0;
  for (i=0; i < n; i++)
  {
    result += f[i] * 1.0/tan(M_PI * (x-i)/n);
    i++;
    result -= f[i] * 1.0/tan(M_PI * (x-i)/n);
  }

//  return result * sin(M_PI*x)/n;  breaks for near-integer x
  return result * sin(M_PI*(x - 2.0*floor(0.5*x)))/n;
}



#define SHFT(a,b,c,d) (a)=(b);(b)=(c);(c)=(d);
#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

static void
find_peak(float *f, unsigned n, 
	  double *xmax, float *ymax)
{
  unsigned i;

  // find peak bin
  unsigned imax = std::max_element(f, f+n)-f;

  // Rest of code is hacked version of Numerical Recipes' "brent".
  double ax = imax-1.0;
  double bx = imax;
  double cx = imax+1.0; 
  double tol = 1.0e-8; // >> sqrt of double precision


  int iter;
  double a,b,d,etemp,p,q,r,tol1,tol2,u,v,w,x,xm;
  float fu,fv,fw,fx;
  double e=0.0;
  const int ITMAX = 100;
  const double CGOLD = 0.3819660;
  const double ZEPS = 1.0e-10;

  a=(ax < cx ? ax : cx);
  b=(ax > cx ? ax : cx);
  x=w=v=bx;
  fw=fv=fx=-sinc_interp(f, x, n);
  for (iter=1;iter<=ITMAX;iter++) {
    xm=0.5*(a+b);
    tol2=2.0*(tol1=tol*fabs(x)+ZEPS);
    if (fabs(x-xm) <= (tol2-0.5*(b-a))) 
    {
      *xmax = x;
      *ymax = fx;
      return ;
    }
    if (fabs(e) > tol1) {
      r=(x-w)*(fx-fv);
      q=(x-v)*(fx-fw);
      p=(x-v)*q-(x-w)*r;
      q=2.0*(q-r);
      if (q > 0.0) p = -p;
      q=fabs(q);
      etemp=e;
      e=d;
      if (fabs(p) >= fabs(0.5*q*etemp) || p <= q*(a-x) || p >= q*(b-x))
	d=CGOLD*(e=(x >= xm ? a-x : b-x));
      else {
	d=p/q;
	u=x+d;
	if (u-a < tol2 || b-u < tol2)
	  d=SIGN(tol1,xm-x);
      }
    } else {
      d=CGOLD*(e=(x >= xm ? a-x : b-x));
    }
    u=(fabs(d) >= tol1 ? x+d : x+SIGN(tol1,d));
    fu=-sinc_interp(f, u, n);
    if (fu <= fx) {
      if (u >= x) a=x; else b=x;
      SHFT(v,w,x,u)
	SHFT(fv,fw,fx,fu)
	} else {
	  if (u < x) a=u; else b=u;
	  if (fu <= fw || w == x) {
	    v=w;
	    w=u;
	    fv=fw;
	    fw=fu;
	  } else if (fu <= fv || v == x || v == w) {
	    v=u;
	    fv=fu;
	  }
	}
  }
  fprintf(stderr, "Internal error!!\n");
  exit(1);
}


  

Estimate<double>
Pulsar::SincInterpShift (const Profile& std, const Profile& obs)
{
  unsigned i, nbin = obs.get_nbin(), ncoeff=nbin/2+2;

  // compute the cross-correlation
  std::complex<float> *obs_spec = new std::complex<float> [ncoeff];
  std::complex<float> *std_spec = new std::complex<float> [ncoeff];
  std::complex<float> *ccf_spec = new std::complex<float> [ncoeff];
  const std::complex<float> zero(0.0, 0.0); 
  float *ccf = new float [nbin];

  fft::frc1d (nbin, (float*)obs_spec, obs.get_amps());
  fft::frc1d (nbin, (float*)std_spec, std.get_amps());

  // Zap harmonics of periodic spikes if necessary
  if (Pulsar::Profile::SIS_zap_period > 1)
  {
    int freq = nbin / Pulsar::Profile::SIS_zap_period;
    for (i=freq; i < ncoeff; i+=freq)
      obs_spec[i] = zero;
  }

  for (i=1; i < ncoeff; i++)
    ccf_spec[i] = obs_spec[i]*conj(std_spec[i]);

  ccf_spec[0] = zero; // ignore DC components
  ccf_spec[nbin/2+1] = zero; // Ignore Nyquist, it has no phase information
  
  fft::bcr1d (nbin, ccf, (float*)ccf_spec);

  double maxbin;
  float peakval;
  find_peak(ccf, nbin, &maxbin, &peakval);
 

  delete [] ccf;
  delete [] ccf_spec;
  delete [] std_spec;
  delete [] obs_spec;


  // XXXX get proper error estimate
  double shift = maxbin / nbin;
  if (shift >=0.5)
    shift -= 1.0; 

  return Estimate<double>(shift, 1.0/nbin);
}

