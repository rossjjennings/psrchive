#include "MEAL/Scalar.h"

/*! The class name is used in the output of template classes and methods */
const char* MEAL::Scalar::Name = "Scalar";

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
