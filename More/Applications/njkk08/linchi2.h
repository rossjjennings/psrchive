//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __linchi2_h_
#define __linchi2_h_

#include <vector>

// A set of routines from Numerical Recipes, used for simple
// linear chi-squared fitting

// Returns the value of ln(Gamma(xx)) for xx > 0

double gammln(const double xx);

// Returns the incomplete gamma function P(a,x) evaluated by its
// series representation as gamser. Also returns ln(Gamma(a)) as gln

void gser(double& gamser, const double a, const double x, double& gln);

// Returns the incomplete gamma function Q(a,x) evaluated by its
// continued fraction representation as gammcf. Also returns
// ln(Gamma(a)) as gln

void gcf(double& gammcf, const double a, const double x, double& gln);

// Returns the incomplete gamma function Q(a,x) = 1 - P(a,x)

double gammq(const double a, const double x);


// A simple linear chi-squared fit routine. Fits the given data
// to a line of the form y = a + b*x
// Parameters:
//
//    x, y          : the coordinates of the points
//    sig           : the std deviations of the points
//    siga and sigb : the respective probable uncertainties
//                    associated with a and b
//    chi2          : the chi-squared value
//    q             : goodness of fit probability
//
// If mwt is false, the standard deviations in y are assumed to be
// unknown. q is returned as 1 and chi2 is normalised to unit
// standard deviation on all points.

void lin_chi2_fit(const std::vector<double>& x, const std::vector<double>& y, 
                  const std::vector<double>& sig, 
		  bool mwt, double& a, double& b, double& siga, double& sigb, 
		  double& chi2, double& q);

#endif
