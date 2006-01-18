#include "MEAL/ParameterPolicy.h"
#include "MEAL/Function.h"

/*! By default, ParameterPolicy derived instances are installed in
  the context during construction. */
bool MEAL::ParameterPolicy::auto_install = true;

MEAL::ParameterPolicy::ParameterPolicy (Function* context) :
 FunctionPolicy (context)
{
  if (auto_install && context)
    context->parameter_policy = this;
}

const MEAL::ParameterPolicy*
MEAL::ParameterPolicy::get_parameter_policy (const Function* context) const
{
  return context->parameter_policy;
}

