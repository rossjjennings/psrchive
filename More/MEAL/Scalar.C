#include "MEAL/Scalar.h"
#include "MEAL/Cached.h"
#include "MEAL/NotCached.h"

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Scalar::Name = "Scalar";

MEAL::Scalar::Scalar ()
{
  evaluation_policy = new Cached<Scalar> (this);
}

MEAL::Scalar::Scalar (const Scalar& copy) : Function (copy)
{
  evaluation_policy = new Cached<Scalar> (this);
}

MEAL::Scalar& MEAL::Scalar::operator = (const Scalar& copy)
{
  Function::operator = (copy);
}

void MEAL::Scalar::evaluate (Estimate<double>& value) const
{
  std::vector<double> gradient;

  value = evaluate (&gradient);

  unsigned nparam = get_nparam();

  if (gradient.size() != nparam)
    throw Error (InvalidState, "MEAL::Scalar::evaluate",
		 "gradient.size=%d != nparam=%d", gradient.size(), nparam);

  for (unsigned iparam=0; iparam<nparam; iparam++)
    value.var += gradient[iparam] * gradient[iparam] * get_variance(iparam);

}

void MEAL::Scalar::copy_evaluation_policy (const Scalar* function)
{
  evaluation_policy = function->evaluation_policy;
}

