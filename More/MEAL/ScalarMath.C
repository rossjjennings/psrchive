#include "Calibration/ScalarMath.h"
#include "Calibration/ScalarValue.h"
#include "Calibration/ScalarRoot.h"
#include "Calibration/ScalarSine.h"
#include "Calibration/ScalarCosine.h"
#include "Calibration/ScalarTangent.h"
#include "Calibration/ScalarHypSine.h"
#include "Calibration/ScalarHypCosine.h"
#include "Calibration/ScalarHypTangent.h"
#include "Calibration/ScalarExponential.h"
#include "Calibration/ScalarLogarithm.h"
#include "Calibration/ScalarAbsolute.h"
#include "Calibration/ScalarInverseHypTangent.h"
#include "Calibration/ScalarInverseTangent2.h"
#include "Calibration/ScalarPower.h"

#include "Calibration/SumRule.h"
#include "Calibration/NegationRule.h"
#include "Calibration/ProductRule.h"
#include "Calibration/InverseRule.h"
#include "Calibration/ChainRule.h"
#include "Calibration/RuleMath.h"

Calibration::ScalarMath::ScalarMath (const Estimate<double>& value)
{
  expression = new ScalarValue (value);
}
    
Calibration::ScalarMath::ScalarMath (double value)
{
  expression = new ScalarValue (value);
}

Calibration::ScalarMath::ScalarMath (const ScalarMath& s)
{
  expression = s.expression;
}

Calibration::ScalarMath::ScalarMath (const Reference::To<Scalar>& s)
{
  expression = s;
}

Calibration::ScalarMath::ScalarMath (Scalar& s)
{
  expression = &s;
}

const Calibration::ScalarMath 
Calibration::ScalarMath::operator = (const ScalarMath& s)
{
  if (&s != this)
    expression = s.expression;

  return *this;
}
 
const Calibration::ScalarMath
Calibration::ScalarMath::operator += (const ScalarMath& b)
{
  return operator = (*this + b);
}

const Calibration::ScalarMath
Calibration::ScalarMath::operator -= (const ScalarMath& b)
{
  return operator = (*this - b);
}
    
const Calibration::ScalarMath
Calibration::ScalarMath::operator *= (const ScalarMath& b)
{
  return operator = (*this * b);
}
    
const Calibration::ScalarMath
Calibration::ScalarMath::operator /= (const ScalarMath& b)
{
  return operator = (*this / b);
}


inline double inv (double x) { return 1.0/x; }

const Calibration::ScalarMath
Calibration::operator + (const ScalarMath& a, const ScalarMath& b)
{
  return *Sum<Scalar> (a.expression, b.expression);
}

const Calibration::ScalarMath
Calibration::operator - (const ScalarMath& a, const ScalarMath& b)
{
  return *Difference<Scalar> (a.expression, b.expression);
}

const Calibration::ScalarMath
Calibration::operator * (const ScalarMath& a, const ScalarMath& b)
{
  return *Product<Scalar> (a.expression, b.expression);
}

const Calibration::ScalarMath
Calibration::operator / (const ScalarMath& a, const ScalarMath& b)
{
  return *Quotient<Scalar> (a.expression, b.expression);
}

const Calibration::ScalarMath
Calibration::operator - (const ScalarMath& a)
{
  return *Negation<Scalar> (a.expression);
}

bool Calibration::operator == (const ScalarMath& a, const ScalarMath& b)
{
  return a.evaluate() == b.evaluate();
}

bool Calibration::operator != (const ScalarMath& a, const ScalarMath& b)
{
  return a.evaluate() != b.evaluate();
}

bool Calibration::operator < (const ScalarMath& a, const ScalarMath& b)
{
  return a.evaluate() < b.evaluate();
}

bool Calibration::operator > (const ScalarMath& a, const ScalarMath& b)
{
  return a.evaluate() > b.evaluate();
}

const Calibration::ScalarMath
Calibration::sqrt (const ScalarMath& x)
{
  ScalarRoot* result = new ScalarRoot;
  result->set_model( x.expression );
  return *result;
}


const Calibration::ScalarMath
Calibration::sin (const ScalarMath& x)
{
  ScalarSine* result = new ScalarSine;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::cos (const ScalarMath& x)
{
  ScalarCosine* result = new ScalarCosine;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::tan (const ScalarMath& x)
{
  ScalarTangent* result = new ScalarTangent;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::sinh (const ScalarMath& x)
{
  ScalarHypSine* result = new ScalarHypSine;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::cosh (const ScalarMath& x)
{
  ScalarHypCosine* result = new ScalarHypCosine;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::tanh (const ScalarMath& x)
{
  ScalarHypTangent* result = new ScalarHypTangent;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::abs (const ScalarMath& x)
{
  ScalarAbsolute* result = new ScalarAbsolute;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::exp (const ScalarMath& x)
{
  ScalarExponential* result = new ScalarExponential;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::log (const ScalarMath& x)
{
  ScalarLogarithm* result = new ScalarLogarithm;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::atanh (const ScalarMath& x)
{
  UnaryScalar* result = new ScalarInverseHypTangent;
  result->set_model( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::atan2 (const ScalarMath& y, const ScalarMath& x)
{
  BinaryScalar* result = new ScalarInverseTangent2;
  result->set_arg1( y.expression );
  result->set_arg2( x.expression );
  return *result;
}

const Calibration::ScalarMath
Calibration::pow (const ScalarMath& x, const ScalarMath& y)
{
  BinaryScalar* result = new ScalarPower;
  result->set_arg1( x.expression );
  result->set_arg2( y.expression );
  return *result;
}

//! Evaluate the expression
double Calibration::ScalarMath::evaluate () const
{
  return expression->evaluate();
}

//! Evaluate the expression and its estimated error
Estimate<double> Calibration::ScalarMath::get_Estimate () const
{
  Estimate<double> value;
  expression->evaluate( value );
  return value;
}
