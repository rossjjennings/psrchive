/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "SmoothingSpline.h"
#include "gcvspl.h"
#include "Error.h"

#include <cassert>

using namespace std;

SmoothingSpline::SmoothingSpline ()
{
  minimize_gcv = true;
  smoothing = -1;
  effective_nfree = 0;
  chi_squared = 0;
  ndat_good = 0;
}

//! Set the smoothing factor
void SmoothingSpline::set_smoothing (double _smooth)
{
  smoothing = _smooth;
  minimize_gcv = false;
}

//! Set the effective number of freely esimated parameters
void SmoothingSpline::set_effective_nfree (double nfree)
{
  effective_nfree = nfree;
  minimize_gcv = false;
}

//! Set the mean squared residual error, MSRE (Equation 5)
void SmoothingSpline::set_msre (double msre)
{
  chi_squared = msre;
  minimize_gcv = false;
}

//! Determine the smoothing factor using generalized cross-validation
void SmoothingSpline::set_minimize_gcv (bool flag)
{
  minimize_gcv = flag;
}

const char* gcvspl_error (int code)
{
  /*
    IER = 0:        Normal exit
    IER = 1:        M.le.0 .or. N.lt.2*M
    IER = 2:        Knot sequence is not strictly
                    increasing, or some weight
                    factor is not positive.
    IER = 3:        Wrong mode  parameter or value.
  */

  switch (code)
    {
    case 0:
      return "Normal exit";
    case 1:
      return "M.le.0 .or. N.lt.2*M";
    case 2:
      return "Knot sequence is not strictly increasing, "
	"or some weight factor is not positive";
    case 3:
      return "Wrong mode parameter or value";
    default:
      return "Unknown error";
    }
}

unsigned SmoothingSpline::get_ndat_good () const { return ndat_good; }

void SmoothingSpline::fit (const vector< double >& data_x,
			   const vector< Estimate<double> >& data_y)
{
  unsigned ndat = data_x.size ();
  
  knot.resize (ndat, 0.0);
  vector<double> ydat (ndat, 0.0);
  vector<double> wdat (ndat, 0.0);

  ndat_good = 0;
  
  for (unsigned idat=0; idat < ndat; idat++)
  {
    if (data_y[idat].var <= 0)
      continue;
	
    knot[ndat_good] = data_x[idat];
    ydat[ndat_good] = data_y[idat].val;
    wdat[ndat_good] = 1.0 / data_y[idat].var;

    ndat_good ++;
  }

  int n = ndat = ndat_good;

  knot.resize (ndat);
  ydat.resize (ndat);
  wdat.resize (ndat);
  
  double* x = &(knot[0]);
  double* y = &(ydat[0]);
  double* wx = &(wdat[0]);
  
  int k = 1;             // one row of y data
  double wydat = 1.0;
  double* wy = &wydat;
  
  // half order: 2 = cubic
  int m = 2;
  
  /* mode:
     1 : val = smoothing, p
     2 : minimize generalized cross-validation (Equation 9)
         (val ignored)
     3 : minimize true predicted mean-squared error (Equation 6) 
         (val = variance = sigma^2 in Equation 4)
     4 : val = number of degrees of freedom
  */
  int mode = 2;
  double val = 0;
  
  if (!minimize_gcv)
  {
    if (smoothing > 0)
    {
      val = smoothing;
      mode = 1;
    }
    else if (effective_nfree > 0)
    {
      assert (effective_nfree < ndat);
      val = ndat - effective_nfree;
      mode = 4;
    }
    else if (chi_squared > 0)
    {
      val = chi_squared;
      mode = 3;
    }
  }
  
  workspace.resize( 6*(n*m+1)+n, 0.0 );
  double* wk = &(workspace[0]);
  
  coefficients.resize ( n, 0.0 );
  double * c = &(coefficients[0]);
  
  /* call Woltring's GCVSPL */
  int err = gcvspl ( x, y, n, wx, wy, m, n, k, mode, val, c, n, wk);
  
  if (err != 0)
    throw Error (FailedCall, "SmoothingSpline::fit",
		 "gcvspl error: %s", gcvspl_error (err));

  current_knot = 0;
  
  /* WK(1) = Generalized Cross Validation value */
  /* WK(2) = Mean Squared Residual. */
  /* WK(3) = Estimate of the number of degrees of */
  /*         freedom of the residual sum of squares */
  /*         per dataset, with 0.lt.WK(3).lt.N-M. */
  /* WK(4) = Smoothing parameter p, multiplicative */
  /*         with the splines' derivative constraint. */
  /* WK(5) = Estimate of the true mean squared error */
  /*         (different formula for |MD| = 3). */
  /* WK(6) = Gauss-Markov error variance. */

#if _DEBUG
  cerr << "GCV           = " << wk[0] << endl;
  cerr << "MSR, R_p      = " << wk[1] << endl;
  cerr << "Trace(I-A_p)  = " << wk[2] << " dof" << endl;
  cerr << "Smoothing, p  = " << wk[3] << endl;
  cerr << "True MSE, T_p = " << wk[4] << endl;
  cerr << "GME           = " << wk[5] << endl;
  cerr << "Test          = " << wk[6] << endl;
#endif
}

double SmoothingSpline::get_fit_gcv ()
{
  return workspace[0];
}

double SmoothingSpline::get_fit_msre ()
{
  return workspace[1];
}

double SmoothingSpline::get_fit_effective_nfree ()
{
  return coefficients.size() - workspace[2];
}

double SmoothingSpline::get_fit_smoothing ()
{
  return workspace[3];
}

double SmoothingSpline::get_fit_true_mse ()
{
  return workspace[4];
}

// Gauss-Markov error variance
double SmoothingSpline::get_fit_Gauss_Markov_error_variance ()
{
  return workspace[4];
}

double SmoothingSpline::evaluate (double xval)
{
  int ider = 0;
  int m = 2;
  int nx = knot.size();
  double* x = &(knot[0]);
  double* c = &(coefficients[0]);
  double* q = &(workspace[6]);

  double yval = splder ( ider, m, nx, xval, x, c, &current_knot, q);

#if _DEBUG
  cerr << "xval=" << xval << " cur=" << current_knot << " yval=" << yval << endl;
#endif
  
  return yval;
}
