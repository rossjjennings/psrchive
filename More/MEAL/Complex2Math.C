#include "MEPL/Complex2Math.h"

#include "MEPL/SumRule.h"
#include "MEPL/NegationRule.h"
#include "MEPL/ProductRule.h"
#include "MEPL/InverseRule.h"
#include "MEPL/ChainRule.h"
#include "MEPL/RuleMath.h"

//! Return a reference to a new SumRule instance representing a+b
Reference::To<Model::Complex2>
operator + (const Reference::To<Model::Complex2>& a,
	    const Reference::To<Model::Complex2>& b)
{
  return Model::Sum<Model::Complex2> (a, b);
}

//! Return a reference to a new SumRule instance representing a-b
Reference::To<Model::Complex2>
operator - (const Reference::To<Model::Complex2>& a,
	    const Reference::To<Model::Complex2>& b)
{
  return Model::Difference<Model::Complex2> (a, b);
}

//! Return a reference to a new ProductRule instance representing a*b
Reference::To<Model::Complex2>
operator * (const Reference::To<Model::Complex2>& a,
	    const Reference::To<Model::Complex2>& b)
{
  return Model::Product<Model::Complex2> (a, b);
}

//! Return a reference to a new ProductRule instance representing a/b
Reference::To<Model::Complex2>
operator / (const Reference::To<Model::Complex2>& a,
	    const Reference::To<Model::Complex2>& b)
{
  return Model::Quotient<Model::Complex2> (a, b);
}
