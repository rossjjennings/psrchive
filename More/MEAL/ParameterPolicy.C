#include "MEAL/ParameterPolicy.h"
#include "MEAL/Function.h"

MEAL::ParameterPolicy::ParameterPolicy (Function* context) :
 FunctionPolicy (context)
{
  if (context)
    context->parameter_policy = this;
}

const MEAL::ParameterPolicy*
MEAL::ParameterPolicy::get_parameter_policy (const Function* context) const
{
  return context->parameter_policy;
}

