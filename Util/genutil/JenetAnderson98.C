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

/*!  Given the fraction of digitized samples in the low voltage state,
  this method returns the optimal values for low and high output
  voltage states, as well as the fractional scattered power

  \param p_in fraction of low voltage state samples
  \retval lo the low voltage output state
  \retval hi the hi voltage output state
  \retval A the fractional scattered power
*/

//! Set the fraction of samples in the low voltage state
/*! This method inverts Eq.45 of JA98.

    Phi is the left-hand side of Eq.44, the fraction of samples
    between x2 and x4; the inverse error function of Phi = alpha.
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
  set_alpha (optimal_threshold / (root2*sigma_n), 0.0);
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
