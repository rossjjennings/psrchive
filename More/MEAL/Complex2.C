/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Complex2.h"
#include "MEAL/Cached.h"

#include <complex>

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Complex2::Name = "Complex2";

MEAL::Complex2::Complex2 ()
{
  evaluation_policy = new Cached<Complex2> (this);
}

MEAL::Complex2::Complex2 (const Complex2& copy) : Function (copy)
{
  evaluation_policy = new Cached<Complex2> (this);
}

MEAL::Complex2& MEAL::Complex2::operator = (const Complex2& copy)
{
  Function::operator = (copy);
}

void MEAL::Complex2::calculate (Complex2* other, Jones<double>& result,
                                std::vector< Jones<double> >* grad)
{
  other->calculate (result, grad);
}

static inline double sqr (double x) { return x*x; }

void MEAL::Complex2::evaluate (Jones<Estimate<double> >& j) const
{
  std::vector< Jones<double> > gradient;

  j = evaluate (&gradient);

  unsigned nparam = get_nparam();

  if (gradient.size() != nparam)
    throw Error (InvalidState, "MEAL::Complex2::evaluate",
		 "gradient.size=%d != nparam=%d", gradient.size(), nparam);

  for (unsigned iparam=0; iparam<nparam; iparam++) {

    double variance = get_variance(iparam);

    for (unsigned i=0; i < j.size(); i++) {
      j[i] += std::complex<Estimate<double> >(Estimate<double>(0.0,sqr( gradient[iparam][i].real() ) * variance),Estimate<double>(0.0,sqr( gradient[iparam][i].imag() ) * variance));

      // This code doesn't work as real() and imag() don't return references
      //j[i].real().var += sqr( gradient[iparam][i].real() ) * variance;
      //j[i].imag().var += sqr( gradient[iparam][i].imag() ) * variance;
    }

  }
}
