#include "Calibration/Complex2Math.h"

#include "Calibration/SumRule.h"
#include "Calibration/NegationRule.h"
#include "Calibration/ProductRule.h"
#include "Calibration/InverseRule.h"
#include "Calibration/ChainRule.h"
#include "Calibration/RuleMath.h"

//! Return a reference to a new SumRule instance representing a+b
Reference::To<Calibration::Complex2>
operator + (const Reference::To<Calibration::Complex2>& a,
	    const Reference::To<Calibration::Complex2>& b)
{
  return Calibration::Sum<Calibration::Complex2> (a, b);
}

//! Return a reference to a new SumRule instance representing a-b
Reference::To<Calibration::Complex2>
operator - (const Reference::To<Calibration::Complex2>& a,
	    const Reference::To<Calibration::Complex2>& b)
{
  return Calibration::Difference<Calibration::Complex2> (a, b);
}

//! Return a reference to a new ProductRule instance representing a*b
Reference::To<Calibration::Complex2>
operator * (const Reference::To<Calibration::Complex2>& a,
	    const Reference::To<Calibration::Complex2>& b)
{
  return Calibration::Product<Calibration::Complex2> (a, b);
}

//! Return a reference to a new ProductRule instance representing a/b
Reference::To<Calibration::Complex2>
operator / (const Reference::To<Calibration::Complex2>& a,
	    const Reference::To<Calibration::Complex2>& b)
{
  return Calibration::Quotient<Calibration::Complex2> (a, b);
}
