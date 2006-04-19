/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "JenetAnderson98.h"
#include "ierf.h"
#include <math.h>
 
/*! From JA98, Table 1 */
const double JenetAnderson98::optimal_threshold = 0.9674;

JenetAnderson98::JenetAnderson98 ()
{
  set_threshold( optimal_threshold );
  hi = lo = A = 0;
}

//! Set the fraction of samples in the low voltage state
/*! This method inverts Eq.45 of JA98.

    Phi is the left-hand side of Eq.44, the fraction of samples
    between x2 and x4; alpha is the inverse error function of Phi.
*/
void JenetAnderson98::set_Phi (double Phi)
{
  set_alpha (ierf(Phi), Phi);
}

static double root2 = sqrt(2.0);

/*! \f$ \sigma_n = \sigma/\langle\sigma\rangle \f$ and, as defined in
  Eq.38, \f$ t = \langle\sigma\rangle s\f$, where s is the spacing or
  optimal threshold. */
void JenetAnderson98::set_sigma_n (double sigma_n)
{
  set_alpha (threshold / (root2*sigma_n), 0.0);
}

/*! \f$ \alpha= t/(\sqrt{2}\sigma)\f$ is the term in brackets on the
  right-hand side of Eq.45 of JA98 */
void JenetAnderson98::set_alpha (double alpha, double Phi)
{
  static double root_pi = sqrt(M_PI);

  if (Phi == 0.0)
    Phi = erf(alpha);

  double expon = exp (-alpha*alpha);

  // Equation 40 (-y1, y4)
  hi = root2/alpha * sqrt(1.0 + (2.0*alpha/root_pi)*(expon/(1.0-Phi)));

  // Equation 41 (-y2, y3)
  lo = root2/alpha * sqrt(1.0 - (2.0*alpha/root_pi)*(expon/Phi));

  // Equation 43
  double halfrootnum = lo*(1-expon) + hi*expon;
  double num = 2.0 * halfrootnum * halfrootnum;
  A = num / ( M_PI * ((lo*lo-hi*hi)*Phi + hi*hi) );
}

/*! Given the sampling threshold, t, this method computes the mean
    fraction of samples that lay within the thresholds, -t and +t,
    where t = threshold * sigma.
    This is equivalent to:
    <ol>
    <li> Eq.45 (with x4 = -x2 = t [Eq.38]);
    <li> Eq.A2 (with xh = -xl = t); or
    <li> the expectation value of the binomial distribution, Eq.A6
    (cf. http://mathworld.wolfram.com/BinomialDistribution.html, Eqn. 12)
    </ol>

    The variance of Phi is the variance of the binomial distribution, Eq.A6.
    (cf. http://mathworld.wolfram.com/BinomialDistribution.html, Eqn. 17)
*/
void JenetAnderson98::set_threshold (double t)
{
  threshold = t;
  mean_Phi = erf (threshold / sqrt(2.0));
  var_Phi = mean_Phi * (1.0 - mean_Phi);
}

/*! Given the mean fraction of samples that lay within the
    thresholds, -t and +t, this method computes the threshold, where
    threshold = t/sigma.
*/
void JenetAnderson98::set_mean_Phi (double Phi)
{
  threshold = ierf (Phi) * sqrt(2.0);
  mean_Phi = Phi;
  var_Phi = mean_Phi * (1.0 - mean_Phi);
}

static double gammln(double xx)
{
  /* Numerical Recipes */
  static double cof [6]= { 76.18009172947146, -86.50532032941677,
			   24.01409824083091, -1.231739572450155,
			   0.1208650973866179e-2, -0.5395239384953e-5 };
  double x, y, tmp, ser;
  int j;

  y = x = xx;
  tmp = x + 5.5;
  tmp -= (x+0.5) * log(tmp);
  ser = 1.000000000190015;
  for (j=0; j<=5; j++) {
    y++;
    ser += cof[j]/y;
  }
  return -tmp + log(2.5066282746310005 * ser/x);
}

// returns ln(n!)
static double factln(int n) {
  return gammln (n+1.0);
}

using namespace std;

/*!
  \param L is the number of points used to measure Phi
  \retval prob_Phi[i] will contain the probability of Phi = i/L
*/
void JenetAnderson98::get_prob_Phi (unsigned L, vector<float>& prob_Phi)
{
  prob_Phi.resize (L);

  double lnLfact = factln (L);

  double mean_Phi = get_mean_Phi ();

  for (unsigned nlo=0; nlo<L; nlo++) {

    double Phi = double(nlo) / double(L);

    prob_Phi[nlo]  = exp( lnLfact - factln(nlo) - factln(L-nlo) +
			  L * ( log(pow (mean_Phi, Phi)) +
				log(pow (1.0-mean_Phi, 1.0-Phi)) ) );

  }
}
