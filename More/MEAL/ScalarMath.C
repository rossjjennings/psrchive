#include "MEPL/ScalarMath.h"
#include "MEPL/ScalarValue.h"
#include "MEPL/ScalarRoot.h"
#include "MEPL/ScalarSine.h"
#include "MEPL/ScalarCosine.h"
#include "MEPL/ScalarTangent.h"
#include "MEPL/ScalarHypSine.h"
#include "MEPL/ScalarHypCosine.h"
#include "MEPL/ScalarHypTangent.h"
#include "MEPL/ScalarExponential.h"
#include "MEPL/ScalarLogarithm.h"
#include "MEPL/ScalarAbsolute.h"
#include "MEPL/ScalarInverseHypTangent.h"
#include "MEPL/ScalarInverseTangent2.h"
#include "MEPL/ScalarPower.h"

#include "MEPL/SumRule.h"
#include "MEPL/NegationRule.h"
#include "MEPL/ProductRule.h"
#include "MEPL/InverseRule.h"
#include "MEPL/ChainRule.h"
#include "MEPL/RuleMath.h"

Model::ScalarMath::ScalarMath (const Estimate<double>& value)
{
  expression = new ScalarValue (value);
}
    
Model::ScalarMath::ScalarMath (double value)
{
  expression = new ScalarValue (value);
}

Model::ScalarMath::ScalarMath (const ScalarMath& s)
{
  expression = s.expression;
}

Model::ScalarMath::ScalarMath (const Reference::To<Scalar>& s)
{
  expression = s;
}

Model::ScalarMath::ScalarMath (Scalar& s)
{
  expression = &s;
}

const Model::ScalarMath 
Model::ScalarMath::operator = (const ScalarMath& s)
{
  if (&s != this)
    expression = s.expression;

  return *this;
}
 
const Model::ScalarMath
Model::ScalarMath::operator += (const ScalarMath& b)
{
  return operator = (*this + b);
}

const Model::ScalarMath
Model::ScalarMath::operator -= (const ScalarMath& b)
{
  return operator = (*this - b);
}
    
const Model::ScalarMath
Model::ScalarMath::operator *= (const ScalarMath& b)
{
  return operator = (*this * b);
}
    
const Model::ScalarMath
Model::ScalarMath::operator /= (const ScalarMath& b)
{
  return operator = (*this / b);
}


inline double inv (double x) { return 1.0/x; }

const Model::ScalarMath
Model::operator + (const ScalarMath& a, const ScalarMath& b)
{
  return *Sum<Scalar> (a.expression, b.expression);
}

const Model::ScalarMath
Model::operator - (const ScalarMath& a, const ScalarMath& b)
{
  return *Difference<Scalar> (a.expression, b.expression);
}

const Model::ScalarMath
Model::operator * (const ScalarMath& a, const ScalarMath& b)
{
  return *Product<Scalar> (a.expression, b.expression);
}

const Model::ScalarMath
Model::operator / (const ScalarMath& a, const ScalarMath& b)
{
  return *Quotient<Scalar> (a.expression, b.expression);
}

const Model::ScalarMath
Model::operator - (const ScalarMath& a)
{
  return *Negation<Scalar> (a.expression);
}

bool Model::operator == (const ScalarMath& a, const ScalarMath& b)
{
  return a.evaluate() == b.evaluate();
}

bool Model::operator != (const ScalarMath& a, const ScalarMath& b)
{
  return a.evaluate() != b.evaluate();
}

bool Model::operator < (const ScalarMath& a, const ScalarMath& b)
{
  return a.evaluate() < b.evaluate();
}

bool Model::operator > (const ScalarMath& a, const ScalarMath& b)
{
  return a.evaluate() > b.evaluate();
}

const Model::ScalarMath
Model::sqrt (const ScalarMath& x)
{
  ScalarRoot* result = new ScalarRoot;
  result->set_model( x.expression );
  return *result;
}


const Model::ScalarMath
Model::sin (const ScalarMath& x)
{
  ScalarSine* result = new ScalarSine;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::cos (const ScalarMath& x)
{
  ScalarCosine* result = new ScalarCosine;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::tan (const ScalarMath& x)
{
  ScalarTangent* result = new ScalarTangent;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::sinh (const ScalarMath& x)
{
  ScalarHypSine* result = new ScalarHypSine;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::cosh (const ScalarMath& x)
{
  ScalarHypCosine* result = new ScalarHypCosine;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::tanh (const ScalarMath& x)
{
  ScalarHypTangent* result = new ScalarHypTangent;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::abs (const ScalarMath& x)
{
  ScalarAbsolute* result = new ScalarAbsolute;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::exp (const ScalarMath& x)
{
  ScalarExponential* result = new ScalarExponential;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::log (const ScalarMath& x)
{
  ScalarLogarithm* result = new ScalarLogarithm;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::atanh (const ScalarMath& x)
{
  UnaryScalar* result = new ScalarInverseHypTangent;
  result->set_model( x.expression );
  return *result;
}

const Model::ScalarMath
Model::atan2 (const ScalarMath& y, const ScalarMath& x)
{
  BinaryScalar* result = new ScalarInverseTangent2;
  result->set_arg1( y.expression );
  result->set_arg2( x.expression );
  return *result;
}

const Model::ScalarMath
Model::pow (const ScalarMath& x, const ScalarMath& y)
{
  BinaryScalar* result = new ScalarPower;
  result->set_arg1( x.expression );
  result->set_arg2( y.expression );
  return *result;
}

//! Evaluate the expression
double Model::ScalarMath::evaluate () const
{
  return expression->evaluate();
}

//! Evaluate the expression and its estimated error
Estimate<double> Model::ScalarMath::get_Estimate () const
{
  Estimate<double> value;
  expression->evaluate( value );
  return value;
}
