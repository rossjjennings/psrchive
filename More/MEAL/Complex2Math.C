#include "MEAL/Complex2Math.h"

#include "MEAL/SumRule.h"
#include "MEAL/NegationRule.h"
#include "MEAL/ProductRule.h"
#include "MEAL/InverseRule.h"
#include "MEAL/ChainRule.h"
#include "MEAL/RuleMath.h"

//! Return a reference to a new SumRule instance representing a+b
Reference::To<MEAL::Complex2>
operator + (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b)
{
  return MEAL::Sum<MEAL::Complex2> (a, b);
}

//! Return a reference to a new SumRule instance representing a-b
Reference::To<MEAL::Complex2>
operator - (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b)
{
  return MEAL::Difference<MEAL::Complex2> (a, b);
}

//! Return a reference to a new ProductRule instance representing a*b
Reference::To<MEAL::Complex2>
operator * (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b)
{
  return MEAL::Product<MEAL::Complex2> (a, b);
}

//! Return a reference to a new ProductRule instance representing a/b
Reference::To<MEAL::Complex2>
operator / (const Reference::To<MEAL::Complex2>& a,
	    const Reference::To<MEAL::Complex2>& b)
{
  return MEAL::Quotient<MEAL::Complex2> (a, b);
}
